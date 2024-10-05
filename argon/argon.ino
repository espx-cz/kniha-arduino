extern "C" {
#include <argon2.h>
}

const char *password = "moje_tajne_heslo";
const char *salt = "random_salt"; // V reálné aplikaci by mělo být náhodné a unikátní pro každý hash

void setup() {
    Serial.begin(115200);

    // Délka výsledného hashovaného hesla
    const size_t hashlen = 32;
    unsigned char hash[hashlen];

    // Parametry Argon2
    uint32_t t_cost = 2;           // Počet iterací
    uint32_t m_cost = (1 << 16);   // Paměťová náročnost v KiB
    uint32_t parallelism = 1;      // Počet paralelních vláken

    // Hashování hesla pomocí Argon2
    int ret = argon2d_hash_raw(t_cost, m_cost, parallelism,
                               password, strlen(password),
                               salt, strlen(salt),
                               hash, hashlen);

    if (ret != ARGON2_OK) {
        Serial.println("Hashování hesla selhalo");
        return;
    }

    // Výpis hashovaného hesla
    Serial.print("Hashované heslo: ");
    for (size_t i = 0; i < hashlen; i++) {
        Serial.printf("%02x", hash[i]);
    }
    Serial.println();

    // Ověření hesla (v reálné aplikaci by se použilo při přihlašování)
    unsigned char verify_hash[hashlen];
    ret = argon2d_hash_raw(t_cost, m_cost, parallelism,
                           password, strlen(password),
                           salt, strlen(salt),
                           verify_hash, hashlen);

    if (ret != ARGON2_OK) {
        Serial.println("Ověření hesla selhalo");
        return;
    }

    // Porovnání hashů
    if (memcmp(hash, verify_hash, hashlen) == 0) {
        Serial.println("Ověření hesla bylo úspěšné");
    } else {
        Serial.println("Ověření hesla selhalo");
    }
}

void loop() {
    // nic nedělá
}
