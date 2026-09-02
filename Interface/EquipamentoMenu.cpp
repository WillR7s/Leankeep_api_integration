#include "EquipamentoMenu.h"

#include <iostream>


// ============================================================
// ESCOLHA DO EQUIPAMENTO
// ============================================================

std::string escolherEquipamento()
{
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


    int escolha;


    std::cout
        << "\nEscolha o equipamento: ";

    std::cin
        >> escolha;


    switch (escolha)
    {
        case 1:

            return "ATENDIMENTO CHAMADO/ESCRITORIO";


        case 2:

            return "CRIPTOGRAFIA HD/CAMPO";


        case 3:

            return "DOCUMENTACAO/CAMPO";


        case 4:

            return "ESTACAO DE TRABALHO 01";


        case 5:

            return "MSLH_SERVER/SERVIDOR";


        default:

            std::cout
                << "\nOpcao de equipamento invalida.\n";

            return "";
    }
}