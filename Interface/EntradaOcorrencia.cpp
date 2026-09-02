#include "EntradaOcorrencia.h"

#include <iostream>
#include <limits>


// ============================================================
// OBTÉM DESCRIÇÃO DA OCORRÊNCIA
// ============================================================

std::string obterDescricaoOcorrencia()
{
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );


    std::string descricao;


    std::cout
        << "\nDigite a descricao da ocorrencia: ";


    std::getline(
        std::cin,
        descricao
    );


    return descricao;
}