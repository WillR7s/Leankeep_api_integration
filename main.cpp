#include <iostream>
#include <string>
#include <limits>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

#include "CriarOcorrencia.h"

#include "LeanKeep/Autenticacao.h"
#include "LeanKeep/Equipamentos.h"
#include "LeanKeep/Usuarios.h"

#include "Interface/TipoOcorrencia.h"
#include "Interface/EquipamentoMenu.h"
#include "Configuracao/LeankeepConfig.h"
#include "Interface/EntradaOcorrencia.h"

#include "Planner/EnviarEmail.h"

using json = nlohmann::json;


// ============================================================
// FUNÇÃO PRINCIPAL
// ============================================================

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout
        << "=====================================\n";

    std::cout
        << "       INTEGRACAO COM LEANKEEP\n";

    std::cout
        << "=====================================\n\n";


    // ========================================================
    // 1. AUTENTICAÇÃO
    // ========================================================

    std::string login;
    std::string senha;

    std::cout << "\nLogin LeanKeep: ";
    std::getline(std::cin, login);

    std::cout << "Senha LeanKeep: ";
    std::getline(std::cin, senha);

    std::string token =
        obterToken(login, senha);

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
        // 4. ESCOLHA DO EQUIPAMENTO
        // ====================================================

        std::string identificadorEquipamento =
            escolherEquipamento();

        if (identificadorEquipamento.empty())
        {
            return 1;
        }


        // ====================================================
        // 5. PROCURA EQUIPAMENTO NA API
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
        // 6. MOSTRA EQUIPAMENTO SELECIONADO
        // ====================================================

        std::cout
            << "\n=====================================\n";

        std::cout
            << "       EQUIPAMENTO SELECIONADO\n";

        std::cout
            << "=====================================\n";


        if (equipamentoEncontrado.contains("equipamento"))
        {
            std::cout
                << "ID: "
                << equipamentoEncontrado["equipamento"]
                << "\n";
        }


        if (equipamentoEncontrado.contains("nome"))
        {
            std::cout
                << "Nome: "
                << equipamentoEncontrado["nome"]
                << "\n";
        }


        if (equipamentoEncontrado.contains("tag"))
        {
            std::cout
                << "Tag: "
                << equipamentoEncontrado["tag"]
                << "\n";
        }


        if (equipamentoEncontrado.contains("site"))
        {
            std::cout
                << "Site ID: "
                << equipamentoEncontrado["site"]
                << "\n";
        }


        if (equipamentoEncontrado.contains("area"))
        {
            std::cout
                << "Area ID: "
                << equipamentoEncontrado["area"]
                << "\n";
        }


        // ====================================================
        // 7. CONSULTA USUÁRIOS
        // ====================================================

        json usuarios =
            obterUsuarios(token);

        if (usuarios.empty())
        {
            std::cout
                << "\nNao foi possivel obter os usuarios.\n";

            return 1;
        }


        std::cout
            << "\nUsuarios consultados com sucesso.\n";

        std::cout
            << "Quantidade de usuarios: "
            << usuarios.size()
            << "\n";


        // ====================================================
        // 8. DESCRIÇÃO DA OCORRÊNCIA
        // ====================================================

        DadosOcorrencia dados;

        dados.descricao =
            obterDescricaoOcorrencia();


        // ====================================================
        // 9. TIPO DA OCORRÊNCIA
        // ====================================================

        dados.tipoAnomalia =
            obterTipoOcorrencia();

        if (dados.tipoAnomalia == 0)
        {
            return 1;
        }


        // ====================================================
        // 10. SOLICITANTE E EXECUTOR
        // ====================================================

        dados.solicitanteId =
            LEANKEEP_USUARIO_PADRAO;

        dados.executorId =
            LEANKEEP_USUARIO_PADRAO;


        // ====================================================
        // 11. TAG DO EQUIPAMENTO
        // ====================================================

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
        // 12. MONTA OCORRÊNCIA
        // ====================================================

        json ocorrencia =
            montarOcorrencia(
                equipamentoEncontrado,
                dados
            );


        // ====================================================
        // 13. MOSTRA JSON DA OCORRÊNCIA
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
        // 14. ENVIA OCORRÊNCIA PARA O LEANKEEP
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
        // 15. RESULTADO E ENVIO PARA POWER AUTOMATE
        // ====================================================

        if (sucesso)
        {
            std::cout
                << "\n========================================\n";

            std::cout
                << "OCORRENCIA CRIADA COM SUCESSO\n";

            std::cout
                << "========================================\n";


            // =================================================
            // TÍTULO DO CARD
            //
            // O ASSUNTO DO E-MAIL SERÁ O TÍTULO DO PLANNER
            // =================================================

            std::string assunto =
                dados.tagEquipamento +
                " - Ocorrencia LeanKeep";


            // =================================================
            // DESCRIÇÃO DO CARD
            //
            // O CORPO DO E-MAIL SERÁ A DESCRIÇÃO/NOTAS
            // =================================================

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
            // CONTA MONITORADA PELO POWER AUTOMATE
            // =================================================

            std::string destinatario =
                "willian.ribeiro09@outlook.com";


            // =================================================
            // ENVIA E-MAIL
            // =================================================

            std::cout
                << "\n========================================\n";

            std::cout
                << "       ENVIANDO E-MAIL\n";

            std::cout
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
                    << "\n========================================\n";

                std::cout
                    << "PROCESSO CONCLUIDO COM SUCESSO\n";

                std::cout
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
                    << "\nOcorrencia criada no LeanKeep,\n";

                std::cout
                    << "mas o e-mail nao foi enviado.\n";
            }
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
            << "\nErro ao interpretar JSON dos equipamentos.\n";

        std::cout
            << erro.what()
            << "\n";

        return 1;
    }


    return 0;
}