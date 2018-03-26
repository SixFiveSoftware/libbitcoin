#include <bitcoin/bitcoin.hpp>
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

ec_compressed getPublicKey() {
  std::string mnemonic = "portion shop border uniform loan grab dismiss boss wild magnet strong supreme era swing else keep voyage forest";
  data_chunk seed = to_chunk(decode_mnemonic(split(mnemonic)));
  wallet::hd_private privateKey = wallet::hd_private(seed, wallet::hd_private::testnet);
  ec_compressed compressedPublicKey = privateKey.to_public().point();
  return compressedPublicKey;
}

operation::list witnessProgram(ec_compressed publicKey) {
  short_hash KeyHash = bitcoin_short_hash(publicKey);
  return {operation(opcode(0)), operation(to_chunk(KeyHash))};
}

int main() {
  // output compressed public key
  std::cout << "Public Key: " << encode_base16(getPublicKey()) << std::endl;

  // output witness program
  script P2WPKH = script(witnessProgram(getPublicKey()));
  std::cout << "Witness Program: " << P2WPKH.to_string(0) << std::endl;

  // create P2SH script
  short_hash WitnessProgramHash = bitcoin_short_hash(P2WPKH.to_data(0));
  script P2SH_P2WPKH = script::to_pay_script_hash_pattern(WitnessProgramHash);

  // print our P2SH script and address
  std::cout << "P2SH Script: " << P2SH_P2WPKH.to_string(0) << std::endl;
  std::cout << "Payment Address: " << payment_address(P2WPKH, payment_address::testnet_p2sh).encoded() << std::endl;
}