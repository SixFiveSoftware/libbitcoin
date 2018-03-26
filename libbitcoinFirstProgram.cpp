#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

using namespace bc;

int main() {
  data_chunk seed(16); // create 16-byte vector
  pseudo_random_fill(seed); // fill it with entropy
  ec_secret secretKey = bitcoin_hash(seed); // create secret key from seed

  std::string hexKey = encode_base16(secretKey); // create base16 encoded string from secret key
  std::cout << "Hex Secret: " << std::endl;
  std::cout << hexKey << std::endl;

  // encode using libbitcoin
  ec_secret rawPrivateKey;
  decode_base16(rawPrivateKey, hexKey);
  wallet::ec_private privateKey(rawPrivateKey, 0x8000, false);
  std::cout << "\nPrivate Key Structure:" << std::endl;
  std::cout << privateKey.encoded() << std::endl;
  ec_secret keyTest = privateKey.secret();
  std::string keyTestString = encode_base16(keyTest);
  if(keyTestString == hexKey) {
    std::cout << "Confirmed: " << keyTestString << std::endl;
  } else {
    std::cout << "Error!" << keyTestString << std::endl;
  }

  // do manually what `privateKey.encoded` above is doing
  hexKey = "80" + hexKey;
  std::cout << "\nVersioned Key: " << std::endl;
  std::cout << hexKey << std::endl;

  data_chunk fullKey;
  data_chunk wifVersion;
  byte_array<37u> versionedKey;
  decode_base16(fullKey, hexKey);
  decode_base16(wifVersion, "0x80");
  build_checked_array(versionedKey, { wifVersion, fullKey });
  std::string practiceKey = encode_base58(versionedKey);
  std::cout << "\nBy Hand: " << std::endl;
  std::cout << practiceKey << std::endl;
  // end manual
}
