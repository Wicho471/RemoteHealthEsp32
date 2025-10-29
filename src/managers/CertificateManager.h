#ifndef CERTIFICATE_MANAGER_H
#define CERTIFICATE_MANAGER_H

#include <Arduino.h>

class CertificateManager {
public:
    CertificateManager();

    bool initialize();

    String getCert() const;
    String getKey() const;

private:
    String _serverCert;
    String _serverKey;
};

#endif //CERTIFICATE_MANAGER_H