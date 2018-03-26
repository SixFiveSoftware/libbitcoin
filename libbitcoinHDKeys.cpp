#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

using namespace bc;

int main() {
  data_chunk seedChunk(16); // create 16-byte vector
  pseudo_random_fill(seedChunk);
  std::cout << "\nHex Seed: " << std::endl;
  std::cout << encode_base16(seedChunk) << std::endl;

  // create mnemonic word list
  wallet::word_list writtenWord = wallet::create_mnemonic(seedChunk);
  if(wallet::validate_mnemonic(writtenWord)) { 
    for (auto i = writtenWord.begin(); i != writtenWord.end(); ++i) {
      std::cout << *i << ' ';
    }
  } else { 
    std::cout << "mnemonic invalid!" << std::endl; 
  }

  // create hd keys
  wallet::hd_private privateKey(seedChunk);
  std::cout << "\n\nMaster Private Key: " << std::endl; // xpriv
  std::cout << privateKey.encoded() << std::endl;

  wallet::hd_key keys = privateKey.to_hd_key();
  std::cout << "\nHex Master Private Key: " << std::endl;
  std::cout << encode_base16(keys) << std::endl;

  // derive public key from private key object's facotry
  wallet::hd_public publicKey = privateKey.to_public();
  std::cout << "\nMaster Public Key: " << std::endl; // xpub
  std::cout << publicKey.encoded() << std::endl;

  // derive child keys
  wallet::hd_private childPrivateKey = privateKey.derive_private(1);
  std::cout << "\nChild Private Key: " << std::endl;
  std::cout << childPrivateKey.encoded() << std::endl;

  wallet::hd_public childPublicKey = privateKey.derive_public(1);
  std::cout << "\nChild Public Key: " << std::endl;
  std::cout << childPublicKey.encoded() << std::endl;

  // get payment address
  std::cout << "\nPayment address: " << std::endl;
  std::cout << wallet::ec_public(childPublicKey.point()).to_payment_address().encoded() << std::endl;
}