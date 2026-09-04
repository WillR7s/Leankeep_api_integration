#include "Autenticacao.h"

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t escreverResposta(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
)
{
    size_t total = size * nmemb;

    std::string* resposta =
        static_cast<std::string*>(userp);

    resposta->append(
        static_cast<char*>(contents),
        total
    );

    return total;
}

std::string obterToken(
    const std::string& login,
    const std::string& senha
)
{
    CURL* curl = curl_easy_init();

    if (!curl)
    {
        std::cerr << "Erro ao inicializar CURL.\n";
        return "";
    }

    std::string resposta;

    curl_mime* mime = curl_mime_init(curl);

    curl_mimepart* part;

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "login");
    curl_mime_data(
        part,
        login.c_str(),
        CURL_ZERO_TERMINATED
    );

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "Password");
    curl_mime_data(
        part,
        senha.c_str(),
        CURL_ZERO_TERMINATED
    );

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "Plataform");
    curl_mime_data(part, "6", CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "ExpireCurrentSession");
   curl_mime_data(part, "true", CURL_ZERO_TERMINATED);;

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "StayConnected");
    curl_mime_data(part, "true", CURL_ZERO_TERMINATED);

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://auth.lkp.app.br/v1/auth/"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_MIMEPOST,
        mime
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

    if (resultado != CURLE_OK)
    {
        std::cerr
            << "Erro na requisicao: "
            << curl_easy_strerror(resultado)
            << "\n";

        curl_mime_free(mime);
        curl_easy_cleanup(curl);

        return "";
    }

    long httpCode = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &httpCode
    );

    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    if (httpCode != 200)
    {
        std::cerr
            << "Falha na autenticacao. HTTP: "
            << httpCode
            << "\n";

        std::cerr
            << "Resposta: "
            << resposta
            << "\n";

        return "";
    }

    try
    {
        json dados = json::parse(resposta);

        if (!dados.contains("authToken"))
        {
            std::cerr
                << "Resposta nao contem authToken.\n";

            return "";
        }

        if (!dados["authToken"].contains("token"))
        {
            std::cerr
                << "Resposta nao contem o token.\n";

            return "";
        }

        std::string token =
            dados["authToken"]["token"];

        std::cout
            << "\nAutenticacao realizada com sucesso.\n";

        return token;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Erro ao interpretar resposta da autenticacao: "
            << e.what()
            << "\n";

        return "";
    }
}