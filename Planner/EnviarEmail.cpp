#include "EnviarEmail.h"
#include "../secret.h"

#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>


struct UploadStatus
{
    size_t bytesLidos = 0;
    const std::string* mensagem = nullptr;
};


// ============================================================
// CALLBACK PARA ENVIAR O CONTEÚDO DO E-MAIL
// ============================================================

static size_t lerMensagem(
    char* buffer,
    size_t tamanho,
    size_t quantidade,
    void* usuario
)
{
    UploadStatus* upload =
        static_cast<UploadStatus*>(usuario);

    size_t capacidade =
        tamanho * quantidade;

    if (
        upload == nullptr ||
        upload->mensagem == nullptr ||
        capacidade == 0
    )
    {
        return 0;
    }

    const std::string& mensagem =
        *upload->mensagem;

    if (upload->bytesLidos >= mensagem.size())
    {
        return 0;
    }

    size_t restante =
        mensagem.size() - upload->bytesLidos;

    size_t quantidadeEnviar =
        (restante < capacidade)
        ? restante
        : capacidade;

    memcpy(
        buffer,
        mensagem.data() + upload->bytesLidos,
        quantidadeEnviar
    );

    upload->bytesLidos += quantidadeEnviar;

    return quantidadeEnviar;
}


// ============================================================
// ENVIA E-MAIL
// ============================================================

bool EnviarEmail(
    const std::string& destinatario,
    const std::string& assunto,
    const std::string& mensagem
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


    // ========================================================
    // CONFIGURAÇÕES
    // ========================================================

    const std::string remetente =
        getGmailRemetente();

    const std::string senha =
        getGmailPassword();


    const std::string smtp =
        "smtps://smtp.gmail.com:465";


    // ========================================================
    // MONTA A MENSAGEM SMTP
    // ========================================================

    std::string email =
        "To: " + destinatario + "\r\n"
        "From: " + remetente + "\r\n"
        "Subject: " + assunto + "\r\n"
        "\r\n"
        + mensagem +
        "\r\n";


    // ========================================================
    // CONTROLE DO UPLOAD
    // ========================================================

    UploadStatus upload;

    upload.bytesLidos = 0;
    upload.mensagem = &email;


    // ========================================================
    // CONFIGURA CURL
    // ========================================================

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        smtp.c_str()
    );


    curl_easy_setopt(
        curl,
        CURLOPT_USERNAME,
        remetente.c_str()
    );


    curl_easy_setopt(
        curl,
        CURLOPT_PASSWORD,
        senha.c_str()
    );


    // ========================================================
    // SSL
    // ========================================================

    curl_easy_setopt(
        curl,
        CURLOPT_USE_SSL,
        CURLUSESSL_ALL
    );


    // ========================================================
    // REMETENTE
    // ========================================================

    curl_easy_setopt(
        curl,
        CURLOPT_MAIL_FROM,
        remetente.c_str()
    );


    // ========================================================
    // DESTINATÁRIO
    // ========================================================

    struct curl_slist* destinatarios =
        nullptr;

    destinatarios =
        curl_slist_append(
            destinatarios,
            destinatario.c_str()
        );


    curl_easy_setopt(
        curl,
        CURLOPT_MAIL_RCPT,
        destinatarios
    );


    // ========================================================
    // CONTEÚDO DO E-MAIL
    // ========================================================

    curl_easy_setopt(
        curl,
        CURLOPT_READFUNCTION,
        lerMensagem
    );


    curl_easy_setopt(
        curl,
        CURLOPT_READDATA,
        &upload
    );


    curl_easy_setopt(
        curl,
        CURLOPT_UPLOAD,
        1L
    );


    // ========================================================
    // DEBUG SMTP
    // ========================================================

    curl_easy_setopt(
        curl,
        CURLOPT_VERBOSE,
        1L
    );


    // ========================================================
    // ENVIA
    // ========================================================

    std::cout
        << "\nEnviando e-mail...\n";
std::cout << "Remetente: " << remetente << "\n";
std::cout << "Tamanho da senha: " << senha.size() << "\n";
    CURLcode resultado =
        curl_easy_perform(curl);


    // ========================================================
    // RESULTADO
    // ========================================================

    if (resultado != CURLE_OK)
    {
        std::cout
            << "\nErro ao enviar e-mail: "
            << curl_easy_strerror(resultado)
            << "\n";

        curl_slist_free_all(
            destinatarios
        );

        curl_easy_cleanup(curl);

        return false;
    }


    std::cout
        << "\nE-mail enviado com sucesso!\n";


    // ========================================================
    // LIMPEZA
    // ========================================================

    curl_slist_free_all(
        destinatarios
    );

    curl_easy_cleanup(curl);


    return true;
}