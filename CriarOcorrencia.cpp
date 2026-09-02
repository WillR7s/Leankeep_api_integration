#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "CriarOcorrencia.h"

using json = nlohmann::json;


// ============================================================
// FUNÇÃO: escreverResposta
// Recebe os dados retornados pela API
// ============================================================

static size_t escreverResposta(
    void* conteudo,
    size_t tamanho,
    size_t quantidade,
    std::string* resposta
)
{
    size_t total = tamanho * quantidade;

    resposta->append(
        static_cast<char*>(conteudo),
        total
    );

    return total;
}


// ============================================================
// FUNÇÃO: montarOcorrencia
// Monta o JSON que será enviado para o LeanKeep
// ============================================================

json montarOcorrencia(
    const json& equipamento,
    const DadosOcorrencia& dados
)
{
    json ocorrencia;


    // --------------------------------------------------------
    // DATA E HORA DA OCORRÊNCIA
    // --------------------------------------------------------

    std::time_t agora =
        std::time(nullptr);

    std::tm* tempoLocal =
        std::localtime(&agora);

    std::ostringstream dataHora;

    dataHora << std::put_time(
        tempoLocal,
        "%Y-%m-%dT%H:%M:%S"
    );

    ocorrencia["dataRegistro"] =
        dataHora.str();


    // --------------------------------------------------------
    // DADOS OBTIDOS DO EQUIPAMENTO
    // --------------------------------------------------------

    ocorrencia["empresaId"] =
        equipamento["empresa"];

    ocorrencia["siteId"] =
        equipamento["site"];

    ocorrencia["equipamentoId"] =
        equipamento["equipamento"];

    ocorrencia["sistemaEmpresaId"] =
        equipamento["sistemaEmpresa"];

    ocorrencia["areaId"] =
        equipamento["area"];

    ocorrencia["tipoEquipamentoId"] =
        equipamento["tipoEquipamento"];


    // --------------------------------------------------------
    // DADOS DA OCORRÊNCIA
    // --------------------------------------------------------

    ocorrencia["tipoAnomalia"] =
        dados.tipoAnomalia;

    ocorrencia["descricao"] =
        dados.descricao;


    // --------------------------------------------------------
    // PLATAFORMA
    // --------------------------------------------------------

    ocorrencia["plataforma"] =
        6;


    // --------------------------------------------------------
    // SOLICITANTE / EMITENTE
    // --------------------------------------------------------

    json emitente;

    emitente["emitenteId"] =
        dados.solicitanteId;

    emitente["emailCadastro"] =
        true;

    emitente["emailCorrecao"] =
        true;


    ocorrencia["emitentes"] =
        json::array();

    ocorrencia["emitentes"].push_back(
        emitente
    );


    // --------------------------------------------------------
    // EXECUTOR / RESPONSÁVEL
    // --------------------------------------------------------

    json executor;

    executor["executorId"] =
        dados.executorId;

    executor["emailCadastro"] =
        false;

    executor["emailCorrecao"] =
        false;


    ocorrencia["executores"] =
        json::array();

    ocorrencia["executores"].push_back(
        executor
    );


    // --------------------------------------------------------
    // VALIDAÇÃO DE DUPLICIDADE
    // --------------------------------------------------------

    ocorrencia["validarDuplicidade"] =
        false;


    return ocorrencia;
}


// ============================================================
// FUNÇÃO: obterTiposOcorrencia
// Consulta os tipos disponíveis no LeanKeep
// ============================================================

json obterTiposOcorrencia(
    const std::string& token
)
{
    CURL* curl =
        curl_easy_init();

    if (!curl)
    {
        std::cout
            << "Erro ao inicializar CURL.\n";

        return {};
    }


    std::string resposta;


    // --------------------------------------------------------
    // Headers
    // --------------------------------------------------------

    struct curl_slist* headers =
        nullptr;


    std::string autorizacao =
        "Authorization: Bearer " + token;


    headers =
        curl_slist_append(
            headers,
            autorizacao.c_str()
        );


    headers =
        curl_slist_append(
            headers,
            "Content-Type: application/json"
        );


    // --------------------------------------------------------
    // URL
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Recebe resposta
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Executa
    // --------------------------------------------------------

    std::cout
        << "\nConsultando tipos de ocorrencia...\n";


    CURLcode resultado =
        curl_easy_perform(curl);


    // --------------------------------------------------------
    // Status HTTP
    // --------------------------------------------------------

    long statusHTTP = 0;


    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    std::cout
        << "Status tipos: "
        << statusHTTP
        << "\n";


    std::cout
        << "Resposta:\n"
        << resposta
        << "\n";


    // --------------------------------------------------------
    // Limpeza
    // --------------------------------------------------------

    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);


    if (resultado != CURLE_OK)
    {
        std::cout
            << "Erro CURL: "
            << curl_easy_strerror(resultado)
            << "\n";

        return {};
    }


    // --------------------------------------------------------
    // Interpreta JSON
    // --------------------------------------------------------

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout
            << "Erro ao interpretar JSON dos tipos.\n";

        return {};
    }
}


// ============================================================
// FUNÇÃO: obterPrioridadesOcorrencia
// Consulta as prioridades da unidade
// ============================================================

json obterPrioridadesOcorrencia(
    const std::string& token
)
{
    CURL* curl =
        curl_easy_init();

    if (!curl)
    {
        std::cout
            << "Erro ao inicializar CURL.\n";

        return {};
    }


    std::string resposta;


    // --------------------------------------------------------
    // Headers
    // --------------------------------------------------------

    struct curl_slist* headers =
        nullptr;


    std::string autorizacao =
        "Authorization: Bearer " + token;


    headers =
        curl_slist_append(
            headers,
            autorizacao.c_str()
        );


    headers =
        curl_slist_append(
            headers,
            "Content-Type: application/json"
        );


    // --------------------------------------------------------
    // URL
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Recebe resposta
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Executa
    // --------------------------------------------------------

    std::cout
        << "\nConsultando prioridades de ocorrencia...\n";


    CURLcode resultado =
        curl_easy_perform(curl);


    // --------------------------------------------------------
    // Status HTTP
    // --------------------------------------------------------

    long statusHTTP = 0;


    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    std::cout
        << "Status prioridades: "
        << statusHTTP
        << "\n";


    std::cout
        << "Resposta:\n"
        << resposta
        << "\n";


    // --------------------------------------------------------
    // Limpeza
    // --------------------------------------------------------

    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);


    if (resultado != CURLE_OK)
    {
        std::cout
            << "Erro CURL: "
            << curl_easy_strerror(resultado)
            << "\n";

        return {};
    }


    // --------------------------------------------------------
    // Interpreta JSON
    // --------------------------------------------------------

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout
            << "Erro ao interpretar JSON das prioridades.\n";

        return {};
    }
}


// ============================================================
// FUNÇÃO: obterUsuarios
// Consulta usuários da unidade
// ============================================================

json obterUsuarios(
    const std::string& token
)
{
    CURL* curl =
        curl_easy_init();

    if (!curl)
    {
        std::cout
            << "Erro ao inicializar CURL.\n";

        return {};
    }


    std::string resposta;


    // --------------------------------------------------------
    // Headers
    // --------------------------------------------------------

    struct curl_slist* headers =
        nullptr;


    std::string autorizacao =
        "Authorization: Bearer " + token;


    headers =
        curl_slist_append(
            headers,
            autorizacao.c_str()
        );


    headers =
        curl_slist_append(
            headers,
            "Content-Type: application/json"
        );


    // --------------------------------------------------------
    // URL
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Recebe resposta
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Executa
    // --------------------------------------------------------

    std::cout
        << "\nConsultando usuarios...\n";


    CURLcode resultado =
        curl_easy_perform(curl);


    // --------------------------------------------------------
    // Status HTTP
    // --------------------------------------------------------

    long statusHTTP = 0;


    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    std::cout
        << "Status usuarios: "
        << statusHTTP
        << "\n";


    std::cout
        << "Resposta:\n"
        << resposta
        << "\n";


    // --------------------------------------------------------
    // Limpeza
    // --------------------------------------------------------

    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);


    if (resultado != CURLE_OK)
    {
        std::cout
            << "Erro CURL: "
            << curl_easy_strerror(resultado)
            << "\n";

        return {};
    }


    // --------------------------------------------------------
    // Interpreta JSON
    // --------------------------------------------------------

    try
    {
        return json::parse(resposta);
    }
    catch (...)
    {
        std::cout
            << "Erro ao interpretar JSON dos usuarios.\n";

        return {};
    }
}


// ============================================================
// FUNÇÃO: enviarOcorrencia
// Envia a ocorrência para o LeanKeep
// ============================================================

bool enviarOcorrencia(
    const std::string& token,
    const json& ocorrencia
)
{
    CURL* curl =
        curl_easy_init();


    if (!curl)
    {
        std::cout
            << "Erro ao inicializar CURL.\n";

        return false;
    }


    std::string resposta;


    // --------------------------------------------------------
    // Headers
    // --------------------------------------------------------

    struct curl_slist* headers =
        nullptr;


    std::string autorizacao =
        "Authorization: Bearer " + token;


    headers =
        curl_slist_append(
            headers,
            autorizacao.c_str()
        );


    headers =
        curl_slist_append(
            headers,
            "Content-Type: application/json"
        );


    // --------------------------------------------------------
    // Converte JSON para string
    // --------------------------------------------------------

    std::string corpo =
        ocorrencia.dump(4);


    std::cout
        << "\n========================================\n";

    std::cout
        << "JSON ENVIADO:\n";

    std::cout
        << "========================================\n";


    std::cout
        << corpo
        << "\n";


    std::cout
        << "========================================\n";


    // --------------------------------------------------------
    // Configura POST
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Recebe resposta
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Executa POST
    // --------------------------------------------------------

    CURLcode resultado =
        curl_easy_perform(curl);


    // --------------------------------------------------------
    // Status HTTP
    // --------------------------------------------------------

    long statusHTTP = 0;


    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    // --------------------------------------------------------
    // Mostra resposta
    // --------------------------------------------------------

    std::cout
        << "\n========================================\n";

    std::cout
        << "RESPOSTA DA API\n";

    std::cout
        << "========================================\n";


    std::cout
        << "HTTP Status: "
        << statusHTTP
        << "\n\n";


    std::cout
        << resposta
        << "\n";


    // --------------------------------------------------------
    // Limpeza
    // --------------------------------------------------------

    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);


    // --------------------------------------------------------
    // Verifica erro CURL
    // --------------------------------------------------------

    if (resultado != CURLE_OK)
    {
        std::cout
            << "\nErro CURL: "
            << curl_easy_strerror(resultado)
            << "\n";

        return false;
    }


    // --------------------------------------------------------
    // Verifica status HTTP
    // --------------------------------------------------------

    if (
        statusHTTP == 200 ||
        statusHTTP == 201
    )
    {
        std::cout
            << "\nOCORRENCIA CRIADA COM SUCESSO!\n";

        return true;
    }


    std::cout
        << "\nFalha ao criar ocorrencia.\n";


    return false;
}