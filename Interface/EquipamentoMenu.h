#ifndef EQUIPAMENTO_MENU_H
#define EQUIPAMENTO_MENU_H

#include <string>

// Mostra o menu de equipamentos
// e retorna o identificador utilizado
// para localizar o equipamento na API.
//
// Retorna string vazia caso a opção seja inválida.
std::string escolherEquipamento();

#endif