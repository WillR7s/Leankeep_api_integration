const selectEquipamento = document.getElementById("Equipamento");

async function carregarEquipamentos() {

    try {

        const resposta = await fetch(
            "http://localhost:8080/api/equipamentos"
        );

        const equipamentos = await resposta.json();

        if (!resposta.ok) {
            console.error(equipamentos);
            alert(equipamentos.erro || "Erro ao carregar equipamentos.");
            return;
        }

        selectEquipamento.innerHTML = "";

        const opcaoInicial = document.createElement("option");

        opcaoInicial.textContent =
            "Selecione um Equipamento";

        opcaoInicial.disabled = true;
        opcaoInicial.selected = true;

        selectEquipamento.appendChild(opcaoInicial);


        equipamentos.forEach(equipamento => {

            const option = document.createElement("option");

            option.value =
                equipamento.equipamento;

            option.textContent =
                equipamento.nomeFormatted;

            selectEquipamento.appendChild(option);

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


carregarEquipamentos();