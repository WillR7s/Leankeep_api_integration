console.log("Ocorrencia.js carregado!");


// ============================================================
// ELEMENTOS DA PÁGINA
// ============================================================

const selectEquipamento =
    document.getElementById("Equipamento");

const selectTipo =
    document.getElementById("TIPO");

const descricao =
    document.getElementById("mensagem");

const botao =
    document.getElementById("criarOcorrencia");

const mensagemSucesso =
    document.getElementById("mensagemSucesso");


// Esconde a mensagem ao carregar a página
if (mensagemSucesso) {
    mensagemSucesso.classList.remove("mostrar");
}


// ============================================================
// CARREGAR EQUIPAMENTOS
// ============================================================

async function carregarEquipamentos() {

    try {

        const resposta = await fetch(
            "http://localhost:8080/api/equipamentos"
        );

        const equipamentos =
            await resposta.json();


        if (!resposta.ok) {

            console.error(equipamentos);

            alert(
                equipamentos.erro ||
                "Erro ao carregar equipamentos."
            );

            return;
        }


        selectEquipamento.innerHTML = "";


        const opcaoInicial =
            document.createElement("option");


        opcaoInicial.textContent =
            "Selecione um Equipamento";

        opcaoInicial.disabled = true;
        opcaoInicial.selected = true;


        selectEquipamento.appendChild(
            opcaoInicial
        );


        equipamentos.forEach(equipamento => {

            const option =
                document.createElement("option");


            // ID enviado para o MainFlow
            option.value =
                equipamento.equipamento;


            // Nome exibido na tela
            option.textContent =
                equipamento.nomeFormatted;


            selectEquipamento.appendChild(
                option
            );

        });

    }
    catch (erro) {

        console.error(
            "Erro ao carregar equipamentos:",
            erro
        );

        alert(
            "Não foi possível carregar os equipamentos."
        );
    }
}


// ============================================================
// CARREGAR TIPOS
// ============================================================

function carregarTipos() {

    selectTipo.innerHTML = "";


    const opcaoInicial =
        document.createElement("option");


    opcaoInicial.textContent =
        "Selecione um Tipo de ocorrência";

    opcaoInicial.disabled = true;
    opcaoInicial.selected = true;


    selectTipo.appendChild(
        opcaoInicial
    );


    const tipos = [

        {
            nome: "Melhoria",
            id: 23
        },

        {
            nome: "Preventiva",
            id: 21
        },

        {
            nome: "Anomalia Simples",
            id: 14
        },

        {
            nome: "Anomalia Crítica",
            id: 13
        },

        {
            nome: "Chamado",
            id: 19
        }

    ];


    tipos.forEach(tipo => {

        const option =
            document.createElement("option");


        option.value =
            tipo.id;


        option.textContent =
            tipo.nome;


        selectTipo.appendChild(
            option
        );

    });
}


// ============================================================
// CRIAR OCORRÊNCIA
// ============================================================

botao.addEventListener(
    "click",
    async function (event) {

        event.preventDefault();


        console.log(
            "BOTÃO FOI CLICADO!"
        );


        // ------------------------------------------------------
        // PEGA OS DADOS DA TELA
        // ------------------------------------------------------

        const equipamentoId =
            selectEquipamento.value;


        const tipoAnomalia =
            selectTipo.value;


        const textoDescricao =
            descricao.value.trim();


        console.log(
            "Equipamento:",
            equipamentoId
        );


        console.log(
            "Tipo:",
            tipoAnomalia
        );


        console.log(
            "Descrição:",
            textoDescricao
        );


        // ------------------------------------------------------
        // ESCONDE MENSAGEM ANTERIOR
        // ------------------------------------------------------

        if (mensagemSucesso) {

            mensagemSucesso.classList.remove(
                "mostrar"
            );

        }


        // ------------------------------------------------------
        // VALIDA EQUIPAMENTO
        // ------------------------------------------------------

        if (!equipamentoId) {

            alert(
                "Selecione um equipamento."
            );

            return;
        }


        // ------------------------------------------------------
        // VALIDA TIPO
        // ------------------------------------------------------

        if (!tipoAnomalia) {

            alert(
                "Selecione o tipo da ocorrência."
            );

            return;
        }


        // ------------------------------------------------------
        // VALIDA DESCRIÇÃO
        // ------------------------------------------------------

        if (!textoDescricao) {

            alert(
                "Descreva a ocorrência."
            );

            return;
        }


        // ------------------------------------------------------
        // ENVIA PARA O MAINFLOW
        // ------------------------------------------------------

        try {

            console.log(
                "Enviando ocorrência para o MainFlow..."
            );


            const resposta =
                await fetch(
                    "http://localhost:8080/api/ocorrencia",
                    {
                        method: "POST",

                        headers: {
                            "Content-Type":
                                "application/json"
                        },

                        body: JSON.stringify({

                            equipamentoId:
                                equipamentoId,

                            tipoAnomalia:
                                Number(tipoAnomalia),

                            descricao:
                                textoDescricao

                        })
                    }
                );


            const resultado =
                await resposta.json();


            console.log(
                "Resposta do MainFlow:",
                resultado
            );


            // --------------------------------------------------
            // VERIFICA ERRO
            // --------------------------------------------------

            if (!resposta.ok) {

                alert(
                    resultado.erro ||
                    "Erro ao criar ocorrência."
                );

                return;
            }


            // ==================================================
            // OCORRÊNCIA CRIADA COM SUCESSO
            // ==================================================
            // ==================================================
            // OCORRÊNCIA CRIADA COM SUCESSO
            // ==================================================

            if (mensagemSucesso) {

                mensagemSucesso.innerHTML = `
        <strong>✓ Ocorrência criada com sucesso!</strong>
        <span>Ocorrência registrada no LeanKeep.</span>
    `;

                mensagemSucesso.classList.add("mostrar");

                // Remove a mensagem depois de 4 segundos
                setTimeout(() => {

                    mensagemSucesso.classList.remove("mostrar");

                }, 4000);
            }

            // --------------------------------------------------
            // LIMPA O FORMULÁRIO
            // --------------------------------------------------

            descricao.value = "";

            selectEquipamento.selectedIndex = 0;

            selectTipo.selectedIndex = 0;

        }
        catch (erro) {

            console.error(
                "Erro ao enviar ocorrência:",
                erro
            );


            alert(
                "Não foi possível conectar ao MainFlow."
            );

        }

    }
);


// ============================================================
// INICIALIZAÇÃO
// ============================================================

carregarEquipamentos();

carregarTipos();