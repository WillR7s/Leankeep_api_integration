#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "CriarOcorrencia.h"
#include "secret.h"

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
    CURL* curl = curl_easy_init();

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


    // Login

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


    // Password

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


    // Plataforma

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


    // ExpireCurrentSession

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


    // StayConnected

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

void substituir(
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
//
// Exemplo:
//
// "ESTAÇÃO DE TRABALHO"
// vira
//
// "estacao de trabalho"
// ============================================================

std::string normalizarTexto(
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
    // Converte ASCII para minúsculo
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
// Procura pelo texto dentro da TAG ou do NOME.
//
// A comparação ignora:
// - maiúsculas/minúsculas
// - acentos
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
            tag = equipamento["tag"];
        }

        // ----------------------------------------------------
        // Obtém NOME
        // ----------------------------------------------------

        if (
            equipamento.contains("nome") &&
            equipamento["nome"].is_string()
        )
        {
            nome = equipamento["nome"];
        }

        std::string tagNormalizada =
            normalizarTexto(tag);

        std::string nomeNormalizado =
            normalizarTexto(nome);

        // ----------------------------------------------------
        // Monta a identificação completa
        //
        // Exemplo:
        // tag  = MSLH_SERVER
        // nome = SERVIDOR
        //
        // resultado:
        // MSLH_SERVER/SERVIDOR
        // ----------------------------------------------------

        std::string identificacaoCompleta =
            tag + "/" + nome;

        std::string identificacaoNormalizada =
            normalizarTexto(identificacaoCompleta);

        // ----------------------------------------------------
        // 1. Tenta encontrar pela identificação completa
        // ----------------------------------------------------

        if (
            identificacaoNormalizada == textoNormalizado
        )
        {
            return equipamento;
        }

        // ----------------------------------------------------
        // 2. Procura o texto dentro da identificação completa
        // ----------------------------------------------------

        if (
            identificacaoNormalizada.find(textoNormalizado)
            != std::string::npos
        )
        {
            return equipamento;
        }

        // ----------------------------------------------------
        // 3. Procura pela TAG
        // ----------------------------------------------------

        if (
            tagNormalizada.find(textoNormalizado)
            != std::string::npos
        )
        {
            return equipamento;
        }

        // ----------------------------------------------------
        // 4. Procura pelo NOME
        // ----------------------------------------------------

        if (
            nomeNormalizado.find(textoNormalizado)
            != std::string::npos
        )
        {
            return equipamento;
        }
    }

    return json();
}


// ============================================================
// FUNÇÃO PRINCIPAL
// ============================================================

int main()
{
    std::cout
        << "=====================================\n";

    std::cout
        << "       INTEGRACAO COM LEANKEEP\n";

    std::cout
        << "=====================================\n\n";


    // ========================================================
    // 1. AUTENTICAÇÃO
    // ========================================================

    std::string token =
        obterToken();


    if (token.empty())
    {
        std::cout
            << "\nNao foi possivel obter o JWT.\n";

        return 1;
    }


    // ========================================================
    // 2. CONSULTA EQUIPAMENTOS
    // ========================================================

    std::string resposta =
        obterEquipamentos(token);


    if (resposta.empty())
    {
        std::cout
            << "\nNao foi possivel obter os equipamentos.\n";

        return 1;
    }


    // ========================================================
    // 3. TRANSFORMA RESPOSTA EM JSON
    // ========================================================

    try
    {
        json equipamentos =
            json::parse(resposta);


        if (!equipamentos.is_array())
        {
            std::cout
                << "Erro: a resposta nao e uma lista.\n";

            return 1;
        }


        std::cout
            << "\nJSON dos equipamentos recebido.\n";


        std::cout
            << "Quantidade de equipamentos: "
            << equipamentos.size()
            << "\n";


        // ====================================================
        // 4. MENU DE EQUIPAMENTOS
        // ====================================================

        std::cout
            << "\n=====================================\n";

        std::cout
            << "          EQUIPAMENTOS\n";

        std::cout
            << "=====================================\n\n";


        std::cout
            << "1 - ATENDIMENTO CHAMADO/ESCRITORIO\n";

        std::cout
            << "2 - CRIPTOGRAFIA HD/CAMPO\n";

        std::cout
            << "3 - DOCUMENTACAO/CAMPO\n";

        std::cout
            << "4 - ESTACAO DE TRABALHO 01/ESTACAO DE TRAB...\n";

        std::cout
            << "5 - MSLH_SERVER/SERVIDOR\n";


        int escolhaEquipamento;


        std::cout
            << "\nEscolha o equipamento: ";


        std::cin
            >> escolhaEquipamento;


        if (
            escolhaEquipamento < 1 ||
            escolhaEquipamento > 5
        )
        {
            std::cout
                << "\nOpcao invalida.\n";

            return 1;
        }


        // ====================================================
        // 5. IDENTIFICADOR DO EQUIPAMENTO
        // ====================================================

        std::string identificadorEquipamento;


        switch (escolhaEquipamento)
        {
            case 1:

                identificadorEquipamento =
                    "ATENDIMENTO CHAMADO/ESCRITORIO";

                break;


            case 2:

                identificadorEquipamento =
                    "CRIPTOGRAFIA HD/CAMPO";

                break;


            case 3:

                identificadorEquipamento =
                    "DOCUMENTACAO/CAMPO";

                break;


            case 4:

                identificadorEquipamento =
                    "ESTACAO DE TRABALHO 01";

                break;


            case 5:

                identificadorEquipamento =
                    "MSLH_SERVER/SERVIDOR";

                break;
        }


        // ====================================================
        // 6. PROCURA EQUIPAMENTO NA API
        // ====================================================

        json equipamentoEncontrado =
            encontrarEquipamento(
                equipamentos,
                identificadorEquipamento
            );


        if (equipamentoEncontrado.empty())
        {
            std::cout
                << "\nEquipamento nao encontrado na API.\n";


            std::cout
                << "Identificador procurado: "
                << identificadorEquipamento
                << "\n";


            return 1;
        }


        // ====================================================
        // 7. MOSTRA EQUIPAMENTO SELECIONADO
        // ====================================================

        std::cout
            << "\n=====================================\n";

        std::cout
            << "       EQUIPAMENTO SELECIONADO\n";

        std::cout
            << "=====================================\n";


        if (
            equipamentoEncontrado.contains("equipamento")
        )
        {
            std::cout
                << "ID: "
                << equipamentoEncontrado["equipamento"]
                << "\n";
        }


        if (
            equipamentoEncontrado.contains("nome")
        )
        {
            std::cout
                << "Nome: "
                << equipamentoEncontrado["nome"]
                << "\n";
        }


        if (
            equipamentoEncontrado.contains("tag")
        )
        {
            std::cout
                << "Tag: "
                << equipamentoEncontrado["tag"]
                << "\n";
        }


        if (
            equipamentoEncontrado.contains("site")
        )
        {
            std::cout
                << "Site ID: "
                << equipamentoEncontrado["site"]
                << "\n";
        }


        if (
            equipamentoEncontrado.contains("area")
        )
        {
            std::cout
                << "Area ID: "
                << equipamentoEncontrado["area"]
                << "\n";
        }


        // ====================================================
        // 8. DESCRIÇÃO DA OCORRÊNCIA
        // ====================================================

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );


        DadosOcorrencia dados;


        std::cout
            << "\nDigite a descricao da ocorrencia: ";


        std::getline(
            std::cin,
            dados.descricao
        );


        // ====================================================
        // 9. TIPO DA OCORRÊNCIA
        // ====================================================

        std::cout
            << "\n=====================================\n";

        std::cout
            << "       TIPO DA OCORRENCIA\n";

        std::cout
            << "=====================================\n\n";


        std::cout
            << "1 - Melhoria\n";

        std::cout
            << "2 - Preventiva\n";

        std::cout
            << "3 - Anomalia Simples\n";

        std::cout
            << "4 - Anomalia Critica\n";

        std::cout
            << "5 - Chamado\n";


        int escolhaTipo;


        std::cout
            << "\nEscolha o tipo: ";


        std::cin
            >> escolhaTipo;


        switch (escolhaTipo)
        {
            case 1:

                dados.tipoAnomalia = 23;

                break;


            case 2:

                dados.tipoAnomalia = 21;

                break;


            case 3:

                dados.tipoAnomalia = 14;

                break;


            case 4:

                dados.tipoAnomalia = 13;

                break;


            case 5:

                dados.tipoAnomalia = 19;

                break;


            default:

                std::cout
                    << "\nOpcao de tipo invalida.\n";

                return 1;
        }


        // ====================================================
        // 10. SOLICITANTE E EXECUTOR
        // ====================================================

        dados.solicitanteId =
            91692;


        dados.executorId =
            91692;


        // Guarda a TAG real encontrada na API

        if (
            equipamentoEncontrado.contains("tag") &&
            equipamentoEncontrado["tag"].is_string()
        )
        {
            dados.tagEquipamento =
                equipamentoEncontrado["tag"];
        }
        else
        {
            dados.tagEquipamento =
                identificadorEquipamento;
        }


        // ====================================================
        // 11. MONTA OCORRÊNCIA
        // ====================================================

        json ocorrencia =
            montarOcorrencia(
                equipamentoEncontrado,
                dados
            );


        // ====================================================
        // 12. MOSTRA JSON DA OCORRÊNCIA
        // ====================================================

        std::cout
            << "\n========================================\n";

        std::cout
            << "       OCORRENCIA MONTADA\n";

        std::cout
            << "========================================\n\n";


        std::cout
            << ocorrencia.dump(4)
            << "\n";


        // ====================================================
        // 13. ENVIA OCORRÊNCIA
        // ====================================================

        std::cout
            << "\n========================================\n";

        std::cout
            << "       ENVIANDO OCORRENCIA\n";

        std::cout
            << "========================================\n";


        bool sucesso =
            enviarOcorrencia(
                token,
                ocorrencia
            );


        // ====================================================
        // 14. RESULTADO
        // ====================================================

        if (sucesso)
        {
            std::cout
                << "\n========================================\n";

            std::cout
                << "PROCESSO CONCLUIDO COM SUCESSO\n";

            std::cout
                << "========================================\n";
        }
        else
        {
            std::cout
                << "\n========================================\n";

            std::cout
                << "NAO FOI POSSIVEL CRIAR A OCORRENCIA\n";

            std::cout
                << "========================================\n";
        }
    }
    catch (const json::parse_error& erro)
    {
        std::cout
            << "\nErro ao interpretar os equipamentos.\n";

        std::cout
            << erro.what()
            << "\n";

        return 1;
    }


    return 0;
}