#ifndef _ED25519UTIL_H
#define _ED25519UTIL_H

#include <Arduino.h>

String generateJWT(char *PrivateKey, char *PublicKey, String KeyID, String ProjectID);

#endif