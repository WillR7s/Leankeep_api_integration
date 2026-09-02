#ifndef USUARIOS_H
#define USUARIOS_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================
// USUÁRIOS LEANKEEP
// ============================================================

json obterUsuarios(
    const std::string& token
);

#endif