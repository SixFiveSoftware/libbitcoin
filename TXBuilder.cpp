#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include "HD_Wallet.cpp"

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

std::string getInput2() {
  std::string input;
  getline(cin, input);
  return input;
}

std::string getInput(int preset) {
  if (preset == 1) {
    return "dawn oxygen federal clap endorse aim round receive drill clown option album";
  } else if (preset == 2) {
    return "2"; // index of the child key
  } else if (preset == 3) {
    return "mnrnjVFimDFrNkszzMtecr4yrMKmEuMRbv"; // destination address
  } else if (preset == 4) {
    return "0.6693"; // amount of bitcoin to spend
  } else if (preset == 5) {
    return "599cc7320426d23908713e58040984a98f83b7c18759765695f938792835ded6"; // UTXO hash to spend
  } else if (preset == 6) {
    return "0"; // Output index of UTXO
  } else {
    return getInput2();
  }
}

int main() {
  // import wallet
  std::cout << "Import Wallet via Mnemonic: " << std::endl;
  std::string mnemonic1 = getInput2();
  HD_Wallet wallet1(split(mnemonic1));
  std::cout << "\nChild Index to Spend From: " << std::endl;
  int child = atoi(getInput(2).c_str());
  data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(child).point());

  // get child payment address for depositing testnet coins
  payment_address childAddress = wallet1.childAddress(2);
  std::cout << "==================================" << std::endl;
  wallet1.dumpKeys();
  std::cout << "==================================" << std::endl;

  // make output
  std::cout << "\nEnter Destination Address: " << std::endl;
  std::string destination = getInput2();  // get string input
  payment_address destinationAddy(destination); // convert to payment_address object
  script outputScript = script().to_pay_key_hash_pattern(destinationAddy.hash()); // get output P2KH script from payment_address object

  // get amount to send, convert to satoshis in base10, construct output
  std::cout << "\nEnter Amount (BTC) To Send: " << std::endl;
  std::string btc = getInput2();
  uint64_t satoshis;
  decode_base10(satoshis, btc, 8); // decodes into base10 amount in Satoshis (66930000 vs 0.6693)
  std::cout << "Decoded satoshis: " << satoshis << std::endl;
  output output1(satoshis, outputScript);
  std::cout << "\nAmount: " << encode_base10(output1.value(), 8) << "BTC : Output Script: " << output1.script().to_string(0) << "\n" << std::endl;

  // Get UTXO
  std:: cout << "\nEnter UTXO Hash to Spend: " << std::endl;
  std::string hashString = getInput2();
  std::cout << "\nEnter Output Index: " << std::endl; // output index (vout) is the index of the transaction you can spend
  std::string index = getInput2();
  uint32_t index1 = atoi(index.c_str());
  hash_digest utxoHash;
  decode_hash(utxoHash, hashString);
  output_point utxo(utxoHash, index1);

  // previous locking script
  script lockingScript = script().to_pay_key_hash_pattern(bitcoin_short_hash(pubkey1));
  std::cout << "\nPrevious Locking Script: " << std::endl;
  std::cout << lockingScript.to_string(0) << "\n" << std::endl;

  // make input
  input input1 = input();
  input1.set_previous_output(utxo);
  input1.set_sequence(0xffffffff);

  // build tx
  transaction tx = transaction();
  tx.inputs().push_back(input1);
  tx.outputs().push_back(output1);

  // endorse tx
  endorsement sig;
  if (lockingScript.create_endorsement(sig, wallet1.childPrivateKey(1).secret(), lockingScript, tx, 0u, all)) {
    std::cout << "Signature: " << std::endl;
    std::cout << encode_base16(sig) << "\n" << std::endl;
  }

  // make sig script
  operation::list sigScript;
  sigScript.push_back(operation(sig));
  sigScript.push_back(operation(pubkey1));
  script unlockingScript(sigScript);

  // make signed tx
  tx.inputs()[0].set_script(unlockingScript);
  std::cout << "Raw Transaction: " << std::endl;
  std::cout << encode_base16(tx.to_data()) << std::endl;
}