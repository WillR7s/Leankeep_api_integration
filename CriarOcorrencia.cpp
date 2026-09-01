#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "CriarOcorrencia.h"

using json = nlohmann::json;

static size_t escreverResposta(
    void* contents,
    size_t size,
    size_t nmemb,
    std::string* resposta
) {
    size_t total = size * nmemb;
    resposta->append(
        static_cast<char*>(contents),
        total
    );
    return total;
}


// ============================================================
// MONTA A OCORRÊNCIA
// ============================================================

json montarOcorrencia(const json& equipamento)
{
    json ocorrencia;

    // Dados da empresa e unidade
    ocorrencia["empresaId"] = equipamento["empresa"];
    ocorrencia["siteId"] = equipamento["site"];

    // Plataforma para integração via API
    ocorrencia["plataforma"] = 6;

    // Tipo da ocorrência
    // 19 = Chamado
    ocorrencia["tipoAnomalia"] = 19;

    // Descrição
    ocorrencia["descricao"] = "Ocorrencia de teste via API";

    // Equipamento
    ocorrencia["equipamentoId"] = equipamento["equipamento"];

    // Sistema
    ocorrencia["sistemaEmpresaId"] = equipamento["sistemaEmpresa"];

    // Área
    ocorrencia["areaId"] = equipamento["area"];

    // Tipo de equipamento
    ocorrencia["tipoEquipamentoId"] = equipamento["tipoEquipamento"];

    // Solicitante
    // Equipe São Luiz - Equipe 1
    // ID: 91692
    json emitente;
    emitente["emitenteId"] = 91692;
    emitente["emailCadastro"] = true;
    emitente["emailCorrecao"] = true;

    ocorrencia["emitentes"] = json::array();
    ocorrencia["emitentes"].push_back(emitente);


    // Executor / Responsável
    // Equipe São Luiz - Equipe 1
    // ID: 91692
    json executor;
    executor["executorId"] = 91692;
    executor["emailCadastro"] = false;
    executor["emailCorrecao"] = false;

    ocorrencia["executores"] = json::array();
    ocorrencia["executores"].push_back(executor);


    // Não validar ocorrência duplicada neste primeiro teste
    ocorrencia["validarDuplicidade"] = false;


    return ocorrencia;
}


// ============================================================
// BUSCA TIPOS DE OCORRÊNCIA
// ============================================================

json obterTiposOcorrencia(const std::string& token)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return {};

    std::string resposta;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + token).c_str()
    );

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v1/tiposocorrencias"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        escreverResposta
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &resposta
    );

    CURLcode resultado = curl_easy_perform(curl);

    long status = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status
    );

    std::cout << "\nStatus tipos de ocorrencia: "
              << status << "\n";

    std::cout << resposta << "\n";

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (resultado != CURLE_OK)
    {
        std::cout << "Erro CURL: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        return {};
    }

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout << "Erro ao interpretar JSON.\n";
        return {};
    }
}


// ============================================================
// BUSCA PRIORIDADES
// ============================================================

json obterPrioridadesOcorrencia(const std::string& token)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return {};

    std::string resposta;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + token).c_str()
    );

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v1/prioridadesocorrencias?unidadeId=65067"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        escreverResposta
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &resposta
    );

    CURLcode resultado = curl_easy_perform(curl);

    long status = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status
    );

    std::cout << "\nStatus prioridades: "
              << status << "\n";

    std::cout << resposta << "\n";

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (resultado != CURLE_OK)
    {
        std::cout << "Erro CURL: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        return {};
    }

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout << "Erro ao interpretar JSON.\n";
        return {};
    }
}


// ============================================================
// BUSCA USUÁRIOS
// ============================================================

json obterUsuarios(const std::string& token)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return {};

    std::string resposta;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + token).c_str()
    );

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v1/usuarios?EmpresaId=3554&UnidadeId=65067"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        escreverResposta
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &resposta
    );

    CURLcode resultado = curl_easy_perform(curl);

    long status = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status
    );

    std::cout << "\nStatus usuarios: "
              << status << "\n";

    std::cout << resposta << "\n";

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (resultado != CURLE_OK)
    {
        std::cout << "Erro CURL: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        return {};
    }

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout << "Erro ao interpretar JSON.\n";
        return {};
    }
}


// ============================================================
// ENVIA OCORRÊNCIA
// ============================================================

bool enviarOcorrencia(
    const std::string& token,
    const json& ocorrencia
)
{
    CURL* curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Erro ao iniciar CURL.\n";
        return false;
    }

    std::string resposta;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + token).c_str()
    );

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    std::string corpo = ocorrencia.dump(4);

    std::cout << "\n========================================\n";
    std::cout << "JSON ENVIADO:\n";
    std::cout << "========================================\n";
    std::cout << corpo << "\n";
    std::cout << "========================================\n";


    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v3/ocorrencias"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_POST,
        1L
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        corpo.c_str()
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        escreverResposta
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &resposta
    );

    // NÃO deixar CURLOPT_VERBOSE ativo
    // para não expor informações sensíveis.

    CURLcode resultado = curl_easy_perform(curl);

    long status = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status
    );


    std::cout << "\n========================================\n";
    std::cout << "RESPOSTA DA API\n";
    std::cout << "========================================\n";

    std::cout << "HTTP Status: "
              << status
              << "\n\n";

    std::cout << resposta << "\n";


    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);


    if (resultado != CURLE_OK)
    {
        std::cout << "\nErro CURL: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        return false;
    }


    if (status == 200 || status == 201)
    {
        std::cout << "\nOCORRENCIA CRIADA COM SUCESSO!\n";
        return true;
    }


    std::cout << "\nFalha ao criar ocorrencia.\n";

    return false;
}