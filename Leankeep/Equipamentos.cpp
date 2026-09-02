#include "Equipamentos.h"

#include <iostream>
#include <string>
#include <cctype>

#include <curl/curl.h>


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
// FUNÇÃO: obterEquipamentos
// Consulta os equipamentos ativos
// ============================================================

std::string obterEquipamentos(
    const std::string& token
)
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
    // Header Authorization
    // --------------------------------------------------------

    struct curl_slist* headers =
        nullptr;


    std::string autorizacao =
        "Authorization: Bearer ";

    autorizacao += token;


    headers =
        curl_slist_append(
            headers,
            autorizacao.c_str()
        );


    // --------------------------------------------------------
    // URL
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

    std::cout
        << "\nConsultando equipamentos...\n";


    CURLcode resultado =
        curl_easy_perform(curl);


    if (resultado != CURLE_OK)
    {
        std::cout
            << "Erro na requisicao: "
            << curl_easy_strerror(resultado)
            << "\n";


        curl_slist_free_all(headers);
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
        << "Status da consulta: "
        << statusHTTP
        << "\n";


    if (statusHTTP != 200)
    {
        std::cout
            << "A API nao retornou os equipamentos.\n";


        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return "";
    }


    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);


    return resposta;
}


// ============================================================
// FUNÇÃO: substituir
// Substitui uma sequência de caracteres por outra
// ============================================================

static void substituir(
    std::string& texto,
    const std::string& procurar,
    const std::string& substituirPor
)
{
    size_t pos = 0;


    while (
        (pos = texto.find(procurar, pos))
        != std::string::npos
    )
    {
        texto.replace(
            pos,
            procurar.length(),
            substituirPor
        );


        pos += substituirPor.length();
    }
}


// ============================================================
// FUNÇÃO: normalizarTexto
//
// Remove acentos e converte para minúsculas.
// ============================================================

static std::string normalizarTexto(
    std::string texto
)
{
    // --------------------------------------------------------
    // A
    // --------------------------------------------------------

    substituir(texto, "á", "a");
    substituir(texto, "à", "a");
    substituir(texto, "ã", "a");
    substituir(texto, "â", "a");
    substituir(texto, "ä", "a");

    substituir(texto, "Á", "a");
    substituir(texto, "À", "a");
    substituir(texto, "Ã", "a");
    substituir(texto, "Â", "a");
    substituir(texto, "Ä", "a");


    // --------------------------------------------------------
    // E
    // --------------------------------------------------------

    substituir(texto, "é", "e");
    substituir(texto, "è", "e");
    substituir(texto, "ê", "e");
    substituir(texto, "ë", "e");

    substituir(texto, "É", "e");
    substituir(texto, "È", "e");
    substituir(texto, "Ê", "e");
    substituir(texto, "Ë", "e");


    // --------------------------------------------------------
    // I
    // --------------------------------------------------------

    substituir(texto, "í", "i");
    substituir(texto, "ì", "i");
    substituir(texto, "î", "i");
    substituir(texto, "ï", "i");

    substituir(texto, "Í", "i");
    substituir(texto, "Ì", "i");
    substituir(texto, "Î", "i");
    substituir(texto, "Ï", "i");


    // --------------------------------------------------------
    // O
    // --------------------------------------------------------

    substituir(texto, "ó", "o");
    substituir(texto, "ò", "o");
    substituir(texto, "õ", "o");
    substituir(texto, "ô", "o");
    substituir(texto, "ö", "o");

    substituir(texto, "Ó", "o");
    substituir(texto, "Ò", "o");
    substituir(texto, "Õ", "o");
    substituir(texto, "Ô", "o");
    substituir(texto, "Ö", "o");


    // --------------------------------------------------------
    // U
    // --------------------------------------------------------

    substituir(texto, "ú", "u");
    substituir(texto, "ù", "u");
    substituir(texto, "û", "u");
    substituir(texto, "ü", "u");

    substituir(texto, "Ú", "u");
    substituir(texto, "Ù", "u");
    substituir(texto, "Û", "u");
    substituir(texto, "Ü", "u");


    // --------------------------------------------------------
    // Ç
    // --------------------------------------------------------

    substituir(texto, "ç", "c");
    substituir(texto, "Ç", "c");


    // --------------------------------------------------------
    // Converte para minúsculo
    // --------------------------------------------------------

    for (char& c : texto)
    {
        c = static_cast<char>(
            std::tolower(
                static_cast<unsigned char>(c)
            )
        );
    }


    return texto;
}


// ============================================================
// FUNÇÃO: encontrarEquipamento
//
// Procura pelo texto dentro da identificação completa,
// TAG ou NOME.
//
// Exemplo:
//
// TAG  = MSLH_SERVER
// NOME = SERVIDOR
//
// Identificação:
// MSLH_SERVER/SERVIDOR
// ============================================================

json encontrarEquipamento(
    const json& equipamentos,
    const std::string& texto
)
{
    std::string textoNormalizado =
        normalizarTexto(texto);


    for (const auto& equipamento : equipamentos)
    {
        std::string tag = "";
        std::string nome = "";


        // ----------------------------------------------------
        // Obtém TAG
        // ----------------------------------------------------

        if (
            equipamento.contains("tag") &&
            equipamento["tag"].is_string()
        )
        {
            tag =
                equipamento["tag"];
        }


        // ----------------------------------------------------
        // Obtém NOME
        // ----------------------------------------------------

        if (
            equipamento.contains("nome") &&
            equipamento["nome"].is_string()
        )
        {
            nome =
                equipamento["nome"];
        }


        std::string tagNormalizada =
            normalizarTexto(tag);


        std::string nomeNormalizado =
            normalizarTexto(nome);


        // ----------------------------------------------------
        // Monta identificação completa
        // ----------------------------------------------------

        std::string identificacaoCompleta =
            tag + "/" + nome;


        std::string identificacaoNormalizada =
            normalizarTexto(
                identificacaoCompleta
            );


        // ----------------------------------------------------
        // 1. Identificação completa
        // ----------------------------------------------------

        if (
            identificacaoNormalizada ==
            textoNormalizado
        )
        {
            return equipamento;
        }


        // ----------------------------------------------------
        // 2. Procura dentro da identificação completa
        // ----------------------------------------------------

        if (
            identificacaoNormalizada.find(
                textoNormalizado
            ) != std::string::npos
        )
        {
            return equipamento;
        }


        // ----------------------------------------------------
        // 3. Procura pela TAG
        // ----------------------------------------------------

        if (
            tagNormalizada.find(
                textoNormalizado
            ) != std::string::npos
        )
        {
            return equipamento;
        }


        // ----------------------------------------------------
        // 4. Procura pelo NOME
        // ----------------------------------------------------

        if (
            nomeNormalizado.find(
                textoNormalizado
            ) != std::string::npos
        )
        {
            return equipamento;
        }
    }


    return json();
}