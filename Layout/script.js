const equipamentos = [
    {
        id: 1,
        nome: "CAG 22",
        tag: "CAG-22"
    },
    {
        id: 2,
        nome: "CAG 23",
        tag: "CAG-23"
    },
    {
        id: 3,
        nome: "AHU 01",
        tag: "AHU-01"
    }
];

const selectEquipamento = document.getElementById("Equipamento");

equipamentos.forEach(equipamento => {

    const option = document.createElement("option");

    option.value = equipamento.id;

    option.textContent =
        equipamento.tag + " - " + equipamento.nome;

    selectEquipamento.appendChild(option);
});