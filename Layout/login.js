const btnEntrar = document.getElementById("btnEntrar");

btnEntrar.addEventListener("click", async () => {

    const login = document.getElementById("usuario").value;
    const senha = document.getElementById("senha").value;

    if (!login || !senha) {
        alert("Preencha o usuário e a senha.");
        return;
    }

    btnEntrar.disabled = true;
    btnEntrar.textContent = "Entrando...";

    try {

        const resposta = await fetch("http://localhost:8080/api/login", {
            method: "POST",

            headers: {
                "Content-Type": "application/json"
            },

            body: JSON.stringify({
                login: login,
                senha: senha
            })
        });

        const dados = await resposta.json();

        if (!resposta.ok) {
            alert(dados.erro || "Erro ao realizar login.");
            return;
        }

        if (dados.sucesso) {
            window.location.href = "Ocorrencia.html";
        }

    } catch (erro) {

        console.error(erro);

        alert(
            "Não foi possível conectar ao servidor do MainFlow."
        );

    } finally {

        btnEntrar.disabled = false;
        btnEntrar.textContent = "Entrar";
    }
});