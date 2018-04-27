#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

//Namespace
using namespace bc;
using namespace wallet;
using namespace machine;  // opcode
using namespace chain;    // transaction, inputs, outputs, script

int main() {
  // instantiate tx object
  transaction tx;

  // Version 1
  uint32_t version = 1u;
  tx.set_version(version);

  // print version in serialised format
  auto serialized_version = to_little_endian(tx.version());
  std::cout << encode_base16(to_chunk(serialized_version));

  // previous tx hash
  std::string prev_tx_string_0 = "d863a7924d35db65b280b583593ed03d765d74a9d0738fc2cb3c2c2b3887bcad";
  hash_digest prev_tx_hash_0;
  decode_hash(prev_tx_hash_0, prev_tx_string_0);

  // previous utxo index
  uint32_t index0 = 0;  // index comes from the vout used
  output_point utxo_tospend_0(prev_tx_hash_0, index0);  // this is where i would loop over array of tuples for inputs

  // build input_0 object
  input input_0;
  input_0.set_previous_output(utxo_tospend_0);
  input_0.set_sequence(0xffffffff);

  // additional input objects can be created for additional inputs
  
  // all input objects can then be added to the transaction
  tx.inputs().push_back(input_0);

  // input script will be added later...
  
  // destination address
  auto output_address_raw = "2N8hwP1WmJrFF5QWABn38y63uYLhnJYJYTF";
  payment_address output_address(output_address_raw);

  // create output locking script/scriptPubKey from template:
  operation::list locking_script_0 = script::to_pay_key_hash_pattern(output_address.hash());

  // define output amount
  std::string btc_amount_string_0 = "0.01";
  uint64_t satoshi_amount_0;
  decode_base10(satoshi_amount_0, btc_amount_string_0, btc_decimal_places); // btc_decimal_places = 8

  // create output_0 object
  output output_0(satoshi_amount_0, locking_script_0);
  // repeat for other outputs
  
  // add outputs to tx
  tx.outputs().push_back(output_0);

  // signer: secret > pubkey > address
  auto output_secret
}
