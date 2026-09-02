#ifndef CRIAR_OCORRENCIA_H
#define CRIAR_OCORRENCIA_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


// ============================================================
// DADOS DE ENTRADA DA OCORRÊNCIA
// ============================================================

struct DadosOcorrencia
{
    std::string tagEquipamento;
    std::string descricao;

    int tipoAnomalia;
    int solicitanteId;
    int executorId;
};


// ============================================================
// MONTA A OCORRÊNCIA
// ============================================================

json montarOcorrencia(
    const json& equipamento,
    const DadosOcorrencia& dados
);


// ============================================================
// ENVIA A OCORRÊNCIA
// ============================================================

bool enviarOcorrencia(
    const std::string& token,
    const json& ocorrencia
);


// ============================================================
// CONSULTAS
// ============================================================

json obterTiposOcorrencia(
    const std::string& token
);

json obterPrioridadesOcorrencia(
    const std::string& token
);

json obterUsuarios(
    const std::string& token
);

#endif