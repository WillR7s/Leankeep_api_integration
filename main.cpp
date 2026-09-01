#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "secret.h"

using json = nlohmann::json;


// ============================================================
// FUNÇÃO: escreverResposta
// Recebe os dados enviados pelo servidor e coloca em uma string
// ============================================================

size_t escreverResposta(
    void* conteudo,
    size_t tamanho,
    size_t quantidade,
    void* usuario
)
{
    size_t tamanhoTotal = tamanho * quantidade;

    std::string* resposta = static_cast<std::string*>(usuario);

    resposta->append(
        static_cast<char*>(conteudo),
        tamanhoTotal
    );

    return tamanhoTotal;
}


// ============================================================
// FUNÇÃO: obterToken
// Faz login no LeanKeep e retorna o JWT
// ============================================================

std::string obterToken()
{
    CURL* curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Erro ao inicializar o libcurl.\n";
        return "";
    }


    // Pega login e senha do secret.cpp
    const char* login = getLeankeepLogin();
    const char* senha = getLeankeepPassword();


    // Cria o formulário multipart
    curl_mime* formulario = curl_mime_init(curl);

    if (!formulario)
    {
        std::cout << "Erro ao criar o formulario.\n";

        curl_easy_cleanup(curl);

        return "";
    }


    // --------------------------------------------------------
    // Campo Login
    // --------------------------------------------------------

    curl_mimepart* campoLogin = curl_mime_addpart(formulario);

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
    // Campo Password
    // --------------------------------------------------------

    curl_mimepart* campoSenha = curl_mime_addpart(formulario);

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
    // Campo Plataform
    // --------------------------------------------------------

    curl_mimepart* campoPlataform = curl_mime_addpart(formulario);

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
    // Campo ExpireCurrentSession
    // --------------------------------------------------------

    curl_mimepart* campoExpire = curl_mime_addpart(formulario);

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
    // Campo StayConnected
    // --------------------------------------------------------

    curl_mimepart* campoStayConnected = curl_mime_addpart(formulario);

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
    // Configura a requisição
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


    // String que receberá a resposta
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
    // Executa o login
    // --------------------------------------------------------

    std::cout << "Autenticando no LeanKeep...\n";

    CURLcode resultado = curl_easy_perform(curl);


    if (resultado != CURLE_OK)
    {
        std::cout << "Erro na requisicao: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }


    // --------------------------------------------------------
    // Verifica o status HTTP
    // --------------------------------------------------------

    long statusHTTP;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    std::cout << "Status da autenticacao: "
              << statusHTTP
              << "\n";


    if (statusHTTP != 200)
    {
        std::cout << "Falha na autenticacao.\n";

        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }


    // --------------------------------------------------------
    // Interpreta o JSON
    // --------------------------------------------------------

    try
    {
        json dados = json::parse(resposta);


        if (!dados.contains("authToken"))
        {
            std::cout << "authToken nao encontrado.\n";

            curl_mime_free(formulario);
            curl_easy_cleanup(curl);

            return "";
        }


        if (!dados["authToken"].contains("token"))
        {
            std::cout << "Token nao encontrado.\n";

            curl_mime_free(formulario);
            curl_easy_cleanup(curl);

            return "";
        }


        // Extrai o JWT
        std::string token = dados["authToken"]["token"];


        std::cout << "JWT obtido com sucesso.\n";
        std::cout << "Tamanho do JWT: "
                  << token.length()
                  << " caracteres\n";


        // Libera recursos
        curl_mime_free(formulario);
        curl_easy_cleanup(curl);


        // Retorna o JWT para quem chamou a função
        return token;
    }
    catch (const json::parse_error& erro)
    {
        std::cout << "Erro ao interpretar o JSON:\n";
        std::cout << erro.what() << "\n";

        curl_mime_free(formulario);
        curl_easy_cleanup(curl);

        return "";
    }
}


// ============================================================
// FUNÇÃO: obterEquipamentos
// Recebe o JWT e consulta os equipamentos ativos
// ============================================================

std::string obterEquipamentos(const std::string& token)
{
    CURL* curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Erro ao inicializar o libcurl.\n";
        return "";
    }


    // --------------------------------------------------------
    // Monta o Header de autenticação
    // --------------------------------------------------------

    struct curl_slist* headers = nullptr;

    std::string autorizacao = "Authorization: Bearer ";

    autorizacao += token;


    headers = curl_slist_append(
        headers,
        autorizacao.c_str()
    );


    // --------------------------------------------------------
    // Configura a URL
    // --------------------------------------------------------

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v2/equipamentos/ativos"
    );


    // Adiciona o header Authorization
    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );


    // --------------------------------------------------------
    // Onde será armazenada a resposta
    // --------------------------------------------------------

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
    // Executa a requisição
    // --------------------------------------------------------

    std::cout << "\nConsultando equipamentos...\n";

    CURLcode resultado = curl_easy_perform(curl);


    if (resultado != CURLE_OK)
    {
        std::cout << "Erro na requisicao: "
                  << curl_easy_strerror(resultado)
                  << "\n";

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return "";
    }


    // --------------------------------------------------------
    // Verifica o status HTTP
    // --------------------------------------------------------

    long statusHTTP;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &statusHTTP
    );


    std::cout << "Status da consulta: "
              << statusHTTP
              << "\n";


    // Libera recursos
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);


    return resposta;
}


// ============================================================
// FUNÇÃO PRINCIPAL
// ============================================================

int main()
{
    std::cout << "=====================================\n";
    std::cout << "      INTEGRACAO COM LEANKEEP\n";
    std::cout << "=====================================\n\n";


    // --------------------------------------------------------
    // 1. Obtém o JWT
    // --------------------------------------------------------

    std::string token = obterToken();


    if (token.empty())
    {
        std::cout << "\nNao foi possivel obter o JWT.\n";
        return 1;
    }


    // --------------------------------------------------------
    // 2. Usa o JWT para consultar equipamentos
    // --------------------------------------------------------

    std::string resposta = obterEquipamentos(token);


    if (resposta.empty())
    {
        std::cout << "\nNao foi possivel obter os equipamentos.\n";
        return 1;
    }


    // --------------------------------------------------------
    // 3. Mostra a resposta da API
    // --------------------------------------------------------

    std::cout << "\nResposta da API:\n";
    std::cout << resposta << "\n";


    return 0;
}