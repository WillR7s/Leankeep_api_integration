#ifndef AUTENTICACAO_H
#define AUTENTICACAO_H

#include <string>

// ============================================================
// AUTENTICAÇÃO LEANKEEP
// ============================================================

std::string obterToken(
    const std::string& login,
    const std::string& senha
);

#endif