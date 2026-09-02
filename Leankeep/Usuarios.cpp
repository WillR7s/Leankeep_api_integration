#include "Usuarios.h"

#include <iostream>
#include <string>
#include <curl/curl.h>

using json = nlohmann::json;


// ============================================================
// FUNÇÃO: escreverResposta
// Recebe os dados retornados pela API
// ============================================================

static size_t escreverResposta(
    void* conteudo,
    size_t tamanho,
    size_t quantidade,
    void* usuario
)
{
    size_t total = tamanho * quantidade;

    std::string* resposta =
        static_cast<std::string*>(usuario);

    resposta->append(
        static_cast<char*>(conteudo),
        total
    );

    return total;
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
    // Executa consulta
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