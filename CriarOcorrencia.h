#ifndef CRIAR_OCORRENCIA_H
#define CRIAR_OCORRENCIA_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json montarOcorrencia(const json& equipamento);

bool enviarOcorrencia(
    const std::string& token,
    const json& ocorrencia
);

json obterTiposOcorrencia(
    const std::string& token
);

json obterPrioridadesOcorrencia(
    const std::string& token
);

json obterUsuarios(
    const std::string& token
);


// Consulta as prioridades de ocorrência
json obterPrioridadesOcorrencia(
    const std::string& token
);


#endif