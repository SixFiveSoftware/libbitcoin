#include <bitcoin/bitcoin.hpp>
#include "HD_Wallet.cpp"

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

int main() {
  HD_Wallet wallet1 = HD_Wallet();
  HD_Wallet wallet2 = HD_Wallet();
  HD_Wallet wallet3 = HD_Wallet();

  data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());
  data_chunk pubkey2 = to_chunk(wallet2.childPublicKey(1).point());
  data_chunk pubkey3 = to_chunk(wallet3.childPublicKey(1).point());

  operation::list opList { operation(opcode(82)), operation(pubkey1), operation(pubkey2), operation(pubkey3), operation(opcode(83)), operation(opcode(174)) };
  script multisigScript(opList);
  if (script().is_pay_multisig_pattern(opList)) {
    std::cout << "Is Multisig Pattern!" << std::endl;
  }

  if (multisigScript.is_valid()) {
    std::cout << "Script is Valid!\n" << std::endl;
  } else {
    std::cout << "Script Invalid!\n" << std::endl;
  }
  std::cout << "Redeem Script: \n" << std::endl;
  std::cout << multisigScript.to_string(0) << "\n" << std::endl;
  std::cout << encode_base16(multisigScript.to_data(0)) << "\n" << std::endl;

  short_hash scriptHash = bitcoin_short_hash(multisigScript.to_data(0));
  script pay2ScriptHash = script(multisigScript.to_pay_script_hash_pattern(scriptHash));

  std::cout << "Locking Script: " << std::endl;
  std::cout << pay2ScriptHash.to_string(0) << "\n" << std::endl;

  payment_address multisigAddress(multisigScript);
  std::cout << "Payment Address: " << std::endl;
  std::cout << multisigAddress.encoded() << "\n" << std::endl;
  std::cout << payment_address(scriptHash).encoded() << "\n" << std::endl;

  // distribute 3 mnemonic seeds whose first child key corresponds to this p2sh address
  std::cout << "Private Key Mnemonics: \n" << std::endl;
  std::cout << "Key One: " << std::endl;
  wallet1.displayMnemonic();
  std::cout << "Key Two: " << std::endl;
  wallet2.displayMnemonic();
  std::cout << "Key Three: " << std::endl;
  wallet3.displayMnemonic();
  // std::cout << "Private Key Mnemonics: \n" << std::endl;
  // std::cout << "Key One: " << wallet1.displayMnemonic() << std::endl;
  // std::cout << "Key Two: " << wallet2.displayMnemonic() << std::endl;
  // std::cout << "Key Three: " << wallet3.displayMnemonic() << std::endl;
}