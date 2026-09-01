#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "secret.h"

using json = nlohmann::json;


// ============================================================
// FUNÇÃO: escreverResposta
// Recebe os dados enviados pelo servidor
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


    const char* login = getLeankeepLogin();
    const char* senha = getLeankeepPassword();


    // --------------------------------------------------------
    // Cria o formulario
    // --------------------------------------------------------

    curl_mime* formulario = curl_mime_init(curl);

    if (!formulario)
    {
        std::cout << "Erro ao criar o formulario.\n";

        curl_easy_cleanup(curl);

        return "";
    }


    // Login
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


    // Password
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


    // Plataforma
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


    // ExpireCurrentSession
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


    // StayConnected
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
    // Configura requisicao
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
    // Status HTTP
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
    // Interpreta JSON
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


        std::string token = dados["authToken"]["token"];


        std::cout << "JWT obtido com sucesso.\n";

        std::cout << "Tamanho do JWT: "
                  << token.length()
                  << " caracteres\n";


        curl_mime_free(formulario);
        curl_easy_cleanup(curl);


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
// Consulta os equipamentos ativos
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
    // Cria o header Authorization
    // --------------------------------------------------------

    struct curl_slist* headers = nullptr;

    std::string autorizacao = "Authorization: Bearer ";

    autorizacao += token;


    headers = curl_slist_append(
        headers,
        autorizacao.c_str()
    );


    // --------------------------------------------------------
    // Configura requisicao
    // --------------------------------------------------------

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://lighthousev2.lkp.app.br/v2/equipamentos/ativos"
    );


    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );


    // --------------------------------------------------------
    // Recebe resposta
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
    // Executa consulta
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
    // Status HTTP
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


    if (statusHTTP != 200)
    {
        std::cout << "A API nao retornou os equipamentos.\n";

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return "";
    }


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
    std::cout << "       INTEGRACAO COM LEANKEEP\n";
    std::cout << "=====================================\n\n";


    // --------------------------------------------------------
    // 1. Autenticação
    // --------------------------------------------------------

    std::string token = obterToken();


    if (token.empty())
    {
        std::cout << "\nNao foi possivel obter o JWT.\n";

        return 1;
    }


    // --------------------------------------------------------
    // 2. Consulta equipamentos
    // --------------------------------------------------------

    std::string resposta = obterEquipamentos(token);


    if (resposta.empty())
    {
        std::cout << "\nNao foi possivel obter os equipamentos.\n";

        return 1;
    }


    // --------------------------------------------------------
    // 3. Transforma resposta em JSON
    // --------------------------------------------------------

    try
    {
        json equipamentos = json::parse(resposta);


        std::cout << "\nJSON dos equipamentos recebido.\n";


        // Verifica se realmente recebemos uma lista
        if (!equipamentos.is_array())
        {
            std::cout << "Erro: a resposta nao e uma lista.\n";

            return 1;
        }


        std::cout << "Quantidade de equipamentos: "
                  << equipamentos.size()
                  << "\n";


        // ----------------------------------------------------
        // 4. Percorre os equipamentos
        // ----------------------------------------------------

        std::cout << "\n=====================================\n";
        std::cout << "         EQUIPAMENTOS\n";
        std::cout << "=====================================\n";


        for (const auto& equipamento : equipamentos)
        {
            std::cout << "\n";


            // ID do equipamento
            if (equipamento.contains("equipamento"))
            {
                std::cout << "ID: "
                          << equipamento["equipamento"]
                          << "\n";
            }


            // Nome
            if (equipamento.contains("nome"))
            {
                std::cout << "Nome: "
                          << equipamento["nome"]
                          << "\n";
            }


            // Tag
            if (equipamento.contains("tag"))
            {
                std::cout << "Tag: "
                          << equipamento["tag"]
                          << "\n";
            }


            // Site
            if (equipamento.contains("site"))
            {
                std::cout << "Site ID: "
                          << equipamento["site"]
                          << "\n";
            }


            // Área
            if (equipamento.contains("area"))
            {
                std::cout << "Area ID: "
                          << equipamento["area"]
                          << "\n";
            }


            std::cout << "-------------------------------------\n";
        }
    }
    catch (const json::parse_error& erro)
    {
        std::cout << "\nErro ao interpretar os equipamentos.\n";
        std::cout << erro.what() << "\n";

        return 1;
    }


    return 0;
}