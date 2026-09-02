#ifndef EQUIPAMENTOS_H
#define EQUIPAMENTOS_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Consulta os equipamentos ativos no LeanKeep
std::string obterEquipamentos(
    const std::string& token
);

// Procura um equipamento pelo texto informado
json encontrarEquipamento(
    const json& equipamentos,
    const std::string& texto
);

#endif