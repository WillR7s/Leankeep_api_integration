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