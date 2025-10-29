#include "CertificateManager.h"
#include <LittleFS.h>           
#include "src/utils/Logger.h"  

CertificateManager::CertificateManager() {
}

// Este método ahora se encarga de todo
bool CertificateManager::initialize() {
    if (!LittleFS.begin()) {
        Logger::log("\n[CertManager] ERROR: No se pudo montar el sistema de archivos LittleFS.");
        return false;
    }

    Logger::log("\n[CertManager] Sistema de archivos LittleFS montado.");
    Logger::log("[CertManager] Cargando certificados...");

    File certFile = LittleFS.open("/esp32-public.crt", "r");
    if (!certFile || certFile.isDirectory()) {
        Logger::log("[CertManager] ERROR: Falló al abrir '/esp32-public.crt'");
        return false;
    }
    _serverCert = certFile.readString();
    certFile.close();
    if (_serverCert.isEmpty()) {
        Logger::log("[CertManager] ERROR: El archivo de certificado está vacío.");
        return false;
    }
    Logger::log("[CertManager] Certificado público cargado.");

    File keyFile = LittleFS.open("/esp32-private.key", "r");
    if (!keyFile || keyFile.isDirectory()) {
        Logger::log("[CertManager] ERROR: Falló al abrir '/esp32-private.key'");
        return false;
    }
    _serverKey = keyFile.readString();
    keyFile.close();
    if (_serverKey.isEmpty()) {
        Logger::log("[CertManager] ERROR: El archivo de llave privada está vacío.");
        return false;
    }
    Logger::log("[CertManager] Llave privada cargada.");
    
    Logger::log("[CertManager] Certificados inicializados correctamente.\n");
    return true;
}

String CertificateManager::getCert() const {
    return _serverCert;
}

String CertificateManager::getKey() const {
    return _serverKey;
}