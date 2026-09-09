#include <iostream>
#include <string>
#include <limits>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "CriarOcorrencia.h"

#include "LeanKeep/Autenticacao.h"
#include "LeanKeep/Equipamentos.h"
#include "LeanKeep/Usuarios.h"
#include "secret.h"

#include "Interface/TipoOcorrencia.h"
#include "Interface/EquipamentoMenu.h"
#include "Configuracao/LeankeepConfig.h"
#include "Interface/EntradaOcorrencia.h"

#include "Planner/EnviarEmail.h"
#include "httplib.h"

using json = nlohmann::json;


// ============================================================
// DADOS DA SESSÃO
// ============================================================

std::string tokenAtual;
std::string equipamentosAtual;


// ============================================================
// INICIA SERVIDOR
// ============================================================

void iniciarServidor()
{
    httplib::Server servidor;


    // ========================================================
    // CORS
    // ========================================================

    servidor.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });


    // ========================================================
    // RESPONDE REQUISIÇÕES OPTIONS
    // ========================================================

    servidor.Options(
        R"(.*)",
        [](const httplib::Request&, httplib::Response& res)
        {
            res.status = 204;
        }
    );


    // ========================================================
    // LOGIN
    // ========================================================

    servidor.Post(
        "/api/login",

        [](const httplib::Request& req,
           httplib::Response& res)
        {
            try
            {
                // ------------------------------------------------
                // Lê JSON enviado pelo navegador
                // ------------------------------------------------

                json dados =
                    json::parse(req.body);


                // ------------------------------------------------
                // Verifica login e senha
                // ------------------------------------------------

                if (!dados.contains("login") ||
                    !dados.contains("senha"))
                {
                    res.status = 400;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Login e senha sao obrigatorios."})",
                        "application/json"
                    );

                    return;
                }


                std::string login =
                    dados["login"];

                std::string senha =
                    dados["senha"];


                std::cout
                    << "\nTentativa de login pelo MainFlow: "
                    << login
                    << "\n";


                // ------------------------------------------------
                // Autentica no LeanKeep
                // ------------------------------------------------

                std::string token =
                    obterToken(
                        login,
                        senha
                    );


                // ------------------------------------------------
                // Verifica autenticação
                // ------------------------------------------------

                if (token.empty())
                {
                    res.status = 401;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Login ou senha invalidos."})",
                        "application/json"
                    );

                    return;
                }


                // ------------------------------------------------
                // Guarda o token da sessão
                // ------------------------------------------------

                tokenAtual =
                    token;


                // =================================================
                // CARREGA EQUIPAMENTOS UMA ÚNICA VEZ
                // =================================================

                std::cout
                    << "\nCarregando equipamentos...\n";


                equipamentosAtual =
                    obterEquipamentos(
                        tokenAtual
                    );


                // ------------------------------------------------
                // Verifica se conseguiu carregar
                // ------------------------------------------------

                if (equipamentosAtual.empty())
                {
                    tokenAtual.clear();

                    res.status = 500;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Login realizado, mas nao foi possivel carregar os equipamentos."})",
                        "application/json"
                    );

                    return;
                }


                std::cout
                    << "Equipamentos carregados com sucesso.\n";


                // ------------------------------------------------
                // Login concluído
                // ------------------------------------------------

                json resposta = {
                    {"sucesso", true}
                };


                res.status = 200;

                res.set_content(
                    resposta.dump(),
                    "application/json"
                );
            }

            catch (const std::exception& erro)
            {
                res.status = 400;

                json resposta = {
                    {"sucesso", false},
                    {"erro", erro.what()}
                };

                res.set_content(
                    resposta.dump(),
                    "application/json"
                );
            }
        }
    );


    // ==========================================================
    // EQUIPAMENTOS
    // ==========================================================

    servidor.Get(
        "/api/equipamentos",

        [](const httplib::Request& req,
           httplib::Response& res)
        {
            // ------------------------------------------------
            // Verifica autenticação
            // ------------------------------------------------

            if (tokenAtual.empty())
            {
                res.status = 401;

                res.set_content(
                    R"({"sucesso":false,"erro":"Usuario nao autenticado."})",
                    "application/json"
                );

                return;
            }


            // ------------------------------------------------
            // Verifica se os equipamentos foram carregados
            // ------------------------------------------------

            if (equipamentosAtual.empty())
            {
                res.status = 500;

                res.set_content(
                    R"({"sucesso":false,"erro":"Equipamentos ainda nao foram carregados."})",
                    "application/json"
                );

                return;
            }


            // ------------------------------------------------
            // Retorna o cache
            // ------------------------------------------------

            res.status = 200;

            res.set_content(
                equipamentosAtual,
                "application/json"
            );
        }
    );


    // ==========================================================
    // CRIAR OCORRÊNCIA
    // ==========================================================

    servidor.Post(
        "/api/ocorrencia",

        [](const httplib::Request& req,
           httplib::Response& res)
        {
            try
            {
                // ------------------------------------------------
                // Verifica autenticação
                // ------------------------------------------------

                if (tokenAtual.empty())
                {
                    res.status = 401;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Usuario nao autenticado."})",
                        "application/json"
                    );

                    return;
                }


                // ------------------------------------------------
                // Lê JSON enviado pelo navegador
                // ------------------------------------------------

                json dadosRecebidos =
                    json::parse(req.body);


                // ------------------------------------------------
                // Verifica dados obrigatórios
                // ------------------------------------------------

                if (!dadosRecebidos.contains("equipamentoId") ||
                    !dadosRecebidos.contains("tipoAnomalia") ||
                    !dadosRecebidos.contains("descricao"))
                {
                    res.status = 400;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Equipamento, tipo e descricao sao obrigatorios."})",
                        "application/json"
                    );

                    return;
                }


                // ------------------------------------------------
                // Dados enviados pelo navegador
                // ------------------------------------------------

                std::string equipamentoId =
                    dadosRecebidos["equipamentoId"];


                int tipoAnomalia =
                    dadosRecebidos["tipoAnomalia"];


                std::string descricao =
                    dadosRecebidos["descricao"];


                std::cout
                    << "\n========================================\n"
                    << "          NOVA OCORRENCIA\n"
                    << "========================================\n";


                std::cout
                    << "Equipamento: "
                    << equipamentoId
                    << "\n";


                std::cout
                    << "Tipo: "
                    << tipoAnomalia
                    << "\n";


                std::cout
                    << "Descricao: "
                    << descricao
                    << "\n";


                // =================================================
                // TRANSFORMA CACHE EM JSON
                // =================================================

                json equipamentos =
                    json::parse(equipamentosAtual);


                // =================================================
                // PROCURA O EQUIPAMENTO PELO ID
                // =================================================

                json equipamentoEncontrado;


                for (const auto& equipamento : equipamentos)
                {
                    if (!equipamento.contains("equipamento"))
                    {
                        continue;
                    }


                    if (
                        equipamento["equipamento"].is_number() &&
                        std::to_string(
                            equipamento["equipamento"].get<long long>()
                        ) == equipamentoId
                    )
                    {
                        equipamentoEncontrado =
                            equipamento;

                        break;
                    }
                }


                // ------------------------------------------------
                // Verifica se encontrou
                // ------------------------------------------------

                if (equipamentoEncontrado.empty())
                {
                    res.status = 404;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Equipamento nao encontrado."})",
                        "application/json"
                    );

                    std::cout
                        << "\nEquipamento nao encontrado.\n";

                    return;
                }


                // =================================================
                // DADOS DA OCORRÊNCIA
                // =================================================

                DadosOcorrencia dados;


                dados.descricao =
                    descricao;


                dados.tipoAnomalia =
                    tipoAnomalia;


                // =================================================
                // SOLICITANTE E EXECUTOR
                // =================================================

                dados.solicitanteId =
                    LEANKEEP_USUARIO_PADRAO;


                dados.executorId =
                    LEANKEEP_USUARIO_PADRAO;


                // =================================================
                // TAG DO EQUIPAMENTO
                // =================================================

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
                        equipamentoId;
                }


                // =================================================
                // MONTA OCORRÊNCIA
                // =================================================

                json ocorrencia =
                    montarOcorrencia(
                        equipamentoEncontrado,
                        dados
                    );


                // =================================================
                // MOSTRA JSON
                // =================================================

                std::cout
                    << "\n========================================\n"
                    << "       OCORRENCIA MONTADA\n"
                    << "========================================\n\n";


                std::cout
                    << ocorrencia.dump(4)
                    << "\n";


                // =================================================
                // ENVIA PARA LEANKEEP
                // =================================================

                std::cout
                    << "\n========================================\n"
                    << "       ENVIANDO OCORRENCIA\n"
                    << "========================================\n";


                bool sucesso =
                    enviarOcorrencia(
                        tokenAtual,
                        ocorrencia
                    );


                // =================================================
                // VERIFICA RESULTADO DO LEANKEEP
                // =================================================

                if (!sucesso)
                {
                    res.status = 500;

                    res.set_content(
                        R"({"sucesso":false,"erro":"Nao foi possivel criar a ocorrencia no LeanKeep."})",
                        "application/json"
                    );

                    std::cout
                        << "\nFalha ao criar ocorrencia.\n";

                    return;
                }


                // =================================================
                // OCORRÊNCIA CRIADA
                // =================================================

                std::cout
                    << "\n========================================\n"
                    << "   OCORRENCIA CRIADA COM SUCESSO\n"
                    << "========================================\n";


                // =================================================
                // MONTA E-MAIL
                // =================================================

                std::string assunto =
                    dados.tagEquipamento +
                    " - Ocorrencia LeanKeep";


                std::string corpo =
                    "Equipamento: " +
                    dados.tagEquipamento +
                    "\r\n\r\n"
                    "Descricao do ocorrido:\r\n" +
                    dados.descricao +
                    "\r\n\r\n"
                    "Tipo da ocorrencia: " +
                    std::to_string(dados.tipoAnomalia);


                // =================================================
                // DESTINATÁRIO
                // =================================================

                std::string destinatario =
                    getEmailDestino();


                // =================================================
                // ENVIA E-MAIL
                // =================================================

                std::cout
                    << "\n========================================\n"
                    << "       ENVIANDO E-MAIL\n"
                    << "========================================\n";


                bool emailEnviado =
                    EnviarEmail(
                        destinatario,
                        assunto,
                        corpo
                    );


                // =================================================
                // RESULTADO DO E-MAIL
                // =================================================

                if (emailEnviado)
                {
                    std::cout
                        << "\n========================================\n"
                        << "PROCESSO CONCLUIDO COM SUCESSO\n"
                        << "========================================\n";


                    std::cout
                        << "LeanKeep: OK\n";


                    std::cout
                        << "E-mail: OK\n";


                    std::cout
                        << "Power Automate: aguardando processamento\n";


                    std::cout
                        << "Planner: aguardando criacao do card\n";
                }
                else
                {
                    std::cout
                        << "\nOcorrencia criada no LeanKeep,\n"
                        << "mas o e-mail nao foi enviado.\n";
                }


                // =================================================
                // RESPOSTA PARA O NAVEGADOR
                // =================================================

                json resposta = {
                    {"sucesso", true},
                    {"mensagem", "Ocorrencia criada com sucesso."},
                    {"emailEnviado", emailEnviado}
                };


                res.status = 200;


                res.set_content(
                    resposta.dump(),
                    "application/json"
                );
            }


            catch (const std::exception& erro)
            {
                std::cerr
                    << "\nErro ao criar ocorrencia: "
                    << erro.what()
                    << "\n";


                res.status = 500;


                json resposta = {
                    {"sucesso", false},
                    {"erro", erro.what()}
                };


                res.set_content(
                    resposta.dump(),
                    "application/json"
                );
            }
        }
    );


    // ==========================================================
    // INICIA SERVIDOR
    // ==========================================================

    std::cout
        << "\n========================================\n"
        << "          MAIN FLOW API\n"
        << "========================================\n"
        << "Servidor iniciado em:\n"
        << "http://localhost:8080\n"
        << "========================================\n\n";


    servidor.listen(
        "0.0.0.0",
        8080
    );
}


// ============================================================
// FUNÇÃO PRINCIPAL
// ============================================================

int main()
{

#ifdef _WIN32

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

#endif


    iniciarServidor();


    return 0;
}