#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

points_value getUTXOs(payment_address Addy, uint64_t amount)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet1.libbitcoin.net:19091");
	client::obelisk_client client(connection);

	points_value val1;
	static const auto on_done = [&val1](const points_value& vals) {

		std::cout << "Success: " << vals.value() << std::endl;
		val1 = vals;
	};

	static const auto on_error = [](const code& ec) {
		std::cout << "Error Code: " << ec.message() << std::endl;
	};

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}

	client.blockchain_fetch_unspent_outputs(on_error, on_done, Addy, amount, select_outputs::algorithm::greedy);
	
	client.wait();
	
	
	//return allPoints;
	return val1;


}
void broadcastTX(transaction tx)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet3.libbitcoin.net:19091");
	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}
	
	static const auto on_done = [](const code& ec) {

		std::cout << "Success: " << ec.message() << std::endl;

	};

	static const auto on_error2 = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	client.transaction_pool_broadcast(on_error2, on_done, tx);
	client.wait();
}

hd_private getPrivateKey(std::string walletMnemonic) {
	std::string mnemonic = walletMnemonic;
	data_chunk seed = to_chunk(decode_mnemonic(split(mnemonic)));
	return hd_private(seed, hd_private::testnet);
}
operation::list witnessProgram(ec_compressed publicKey) {
	short_hash KeyHash = bitcoin_short_hash(publicKey);
	return {operation(opcode(0)), operation(to_chunk(KeyHash))};
}

bc::wallet::hd_private childPrivateKey(bc::wallet::hd_private privKey, int index) {
	return privKey.derive_private(index);
}

payment_address paymentAddressForCompressedPubKey(ec_compressed compressedPublicKey, script P2WPKH) {
	// script P2WPKH = script(witnessProgram(compressedPublicKey));
	short_hash WitnessProgramHash = bitcoin_short_hash(P2WPKH.to_data(0));
	payment_address fromAddress = payment_address(P2WPKH, payment_address::testnet_p2sh);
  return fromAddress;
}

int main() {

	// hd_private privateKey = getPrivateKey("retire detect ceiling lab labor approve busy easy swing adjust dumb north");
	hd_private privateKey = getPrivateKey("scout citizen figure wise cute entry stomach spatial salmon diesel spray small");

  auto hardenedOffset = bc::wallet::hd_first_hardened_key;
	int hardenedPurposeIndex = (int)(49 + hardenedOffset);
	int hardenedCoinIndex = (int)(1 + hardenedOffset);
	int hardenedAccountIndex = (int)(0 + hardenedOffset);
	int changeIndex = (int)(0);
	int indexIndex = (int)(0);

	// 2. generate keys
	bc::wallet::hd_private purposePrivateKey = childPrivateKey(privateKey, hardenedPurposeIndex);

	bc::wallet::hd_private coinPrivateKey = childPrivateKey(purposePrivateKey, hardenedCoinIndex);

	bc::wallet::hd_private accountPrivateKey = childPrivateKey(coinPrivateKey, hardenedAccountIndex);

	bc::wallet::hd_private changePrivateKey = childPrivateKey(accountPrivateKey, changeIndex);
	bc::wallet::hd_private receiveChangePrivateKey = childPrivateKey(accountPrivateKey, 1);

	bc::wallet::hd_private indexPrivateKey = childPrivateKey(changePrivateKey, 0);
	bc::wallet::hd_public indexPublicKey = indexPrivateKey.to_public();

	ec_compressed compressedPublicKey = indexPublicKey.point();


	script P2WPKH = script(witnessProgram(compressedPublicKey));
	// short_hash WitnessProgramHash = bitcoin_short_hash(P2WPKH.to_data(0));
	// payment_address fromAddress = payment_address(P2WPKH, payment_address::testnet_p2sh);
  payment_address fromAddress = paymentAddressForCompressedPubKey(compressedPublicKey, P2WPKH);


	//Start Building Transanction
	transaction tx;
	tx.set_version(1u);
	//Make Output
	// payment_address toAddress = wallet::payment_address("2N8hwP1WmJrFF5QWABn38y63uYLhnJYJYTF");
	// payment_address toAddress = wallet::payment_address("2NAAEjA6c7V8o8WVhnfe977ZBu8SUY67bXW");
	payment_address toAddress = wallet::payment_address("2N7CpJsSKeZNabscs3QRBggNWzi48FB8omM");
	uint64_t amount;
	btc_to_satoshi(amount, "0.005");
	tx.outputs().push_back(output(amount, script(script().to_pay_script_hash_pattern(toAddress.hash()))));
	//Make Change
  std::cout << "from address: " << fromAddress.encoded() << std::endl;
  hd_private nextIndexPrivateKey = childPrivateKey(receiveChangePrivateKey, 0);
  ec_compressed compressedChangePublicKey = nextIndexPrivateKey.to_public().point();
  script changeP2WPKH = script(witnessProgram(compressedChangePublicKey));
  payment_address changeAddress = paymentAddressForCompressedPubKey(compressedChangePublicKey, changeP2WPKH);
  std::cout << "change address: " << changeAddress.encoded() << std::endl;

	points_value UTXOs = getUTXOs(fromAddress, amount);
	uint64_t change = UTXOs.value() - amount - 10000;
	script outScript = script(script().to_pay_script_hash_pattern(changeAddress.hash()));
	tx.outputs().push_back(output(change, outScript));

	//Make Input
	input workingInput = input();
	workingInput.set_previous_output(output_point(UTXOs.points[0]));
	workingInput.set_sequence(max_input_sequence);
	tx.inputs().push_back(workingInput);
	uint64_t previous_amount = UTXOs.points[0].value();

	//Make Signature
	script script_code = script::to_pay_key_hash_pattern(bitcoin_short_hash(compressedPublicKey));
	endorsement sig;
	script().create_endorsement(sig, indexPrivateKey.secret(), script_code, tx, tx.inputs()[0].previous_output().index(), sighash_algorithm::all,  script_version::zero, previous_amount);

	//Make Witness
	data_stack witness_data {to_chunk(sig), to_chunk(compressedPublicKey)};
	tx.inputs()[0].set_witness(witness(witness_data));

	//set input script
	data_chunk scriptChunk = to_chunk(P2WPKH.to_data(1));
	tx.inputs()[0].set_script(script(scriptChunk, false));

	std::cout << encode_base16(tx.to_data(1, 1)) << std::endl;

	broadcastTX(tx);

	//build
	//g++ -std=c++11 -o spend segwit_spend.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin libbitcoin-client)
}
