#include "Autenticacao.h"

#include <iostream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "../secret.h"

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
// FUNÇÃO: obterToken
// Faz a autenticação no LeanKeep
// ============================================================

std::string obterToken()
{
    CURL* curl =
        curl_easy_init();

    if (!curl)
    {
        std::cout
            << "Erro ao inicializar o libcurl.\n";

        return "";
    }


    // --------------------------------------------------------
    // Credenciais
    // --------------------------------------------------------

    const char* login =
        getLeankeepLogin();

    const char* senha =
        getLeankeepPassword();


    // --------------------------------------------------------
    // Cria formulário multipart
    // --------------------------------------------------------

    curl_mime* formulario =
        curl_mime_init(curl);


    // --------------------------------------------------------
    // Login
    // --------------------------------------------------------

    curl_mimepart* campoLogin =
        curl_mime_addpart(formulario);

    curl_mime_name(
        campoLogin,
        "login"
    );

    curl_mime_data(
        campoLogin,
        login,
        CURL_ZERO_TERMINATED
    );


    // --------------------------------------------------------
    // Password
    // --------------------------------------------------------

    curl_mimepart* campoSenha =
        curl_mime_addpart(formulario);

    curl_mime_name(
        campoSenha,
        "Password"
    );

    curl_mime_data(
        campoSenha,
        senha,
        CURL_ZERO_TERMINATED
    );


    // --------------------------------------------------------
    // Plataforma
    // --------------------------------------------------------

    curl_mimepart* campoPlataform =
        curl_mime_addpart(formulario);

    curl_mime_name(
        campoPlataform,
        "Plataform"
    );

    curl_mime_data(
        campoPlataform,
        "6",
        CURL_ZERO_TERMINATED
    );


    // --------------------------------------------------------
    // ExpireCurrentSession
    // --------------------------------------------------------

    curl_mimepart* campoExpire =
        curl_mime_addpart(formulario);

    curl_mime_name(
        campoExpire,
        "ExpireCurrentSession"
    );

    curl_mime_data(
        campoExpire,
        "True",
        CURL_ZERO_TERMINATED
    );


    // --------------------------------------------------------
    // StayConnected
    // --------------------------------------------------------

    curl_mimepart* campoStayConnected =
        curl_mime_addpart(formulario);

    curl_mime_name(
        campoStayConnected,
        "StayConnected"
    );

    curl_mime_data(
        campoStayConnected,
        "True",
        CURL_ZERO_TERMINATED
    );


    // --------------------------------------------------------
    // Configura requisição
    // --------------------------------------------------------

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://auth.lkp.app.br/v1/auth/"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_MIMEPOST,
        formulario
    );


    std::string resposta;

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
    // Executa login
    // --------------------------------------------------------

    std::cout
        << "Autenticando no LeanKeep...\n";

    CURLcode resultado =
        curl_easy_perform(curl);


    if (resultado != CURLE_OK)
    {
        std::cout
            << "Erro na requisicao: "
            << curl_easy_strerror(resultado)
            << "\n";

        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }


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
        << "Status da autenticacao: "
        << statusHTTP
        << "\n";


    if (statusHTTP != 200)
    {
        std::cout
            << "Falha na autenticacao.\n";

        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }


    // --------------------------------------------------------
    // Interpreta JSON
    // --------------------------------------------------------

    try
    {
        json dados =
            json::parse(resposta);


        if (!dados.contains("authToken"))
        {
            std::cout
                << "authToken nao encontrado.\n";

            curl_mime_free(formulario);
            curl_easy_cleanup(curl);

            return "";
        }


        if (!dados["authToken"].contains("token"))
        {
            std::cout
                << "Token nao encontrado.\n";

            curl_mime_free(formulario);
            curl_easy_cleanup(curl);

            return "";
        }


        std::string token =
            dados["authToken"]["token"];


        std::cout
            << "JWT obtido com sucesso.\n";

        std::cout
            << "Tamanho do JWT: "
            << token.length()
            << " caracteres\n";


        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return token;
    }
    catch (const json::parse_error& erro)
    {
        std::cout
            << "Erro ao interpretar o JSON:\n";

        std::cout
            << erro.what()
            << "\n";


        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }
}