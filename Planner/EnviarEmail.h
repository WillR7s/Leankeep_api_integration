#ifndef ENVIAR_EMAIL_H
#define ENVIAR_EMAIL_H
#include <string>

bool EnviarEmail(
    const std::string& destinatario,
    const std::string& assunto,
    const std::string& corpo
);

#endif