#include "TipoOcorrencia.h"

#include <iostream>

int obterTipoOcorrencia()
{
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
            return 23;

        case 2:
            return 21;

        case 3:
            return 14;

        case 4:
            return 13;

        case 5:
            return 19;

        default:
            std::cout
                << "\nOpcao de tipo invalida.\n";

            return 0;
    }
}
