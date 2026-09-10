const btnEntrar = document.getElementById("btnEntrar");

const campoUsuario = document.getElementById("usuario");
const campoSenha = document.getElementById("senha");
const lembrarMe = document.getElementById("lembrarMe");

const mensagemErro = document.getElementById("mensagemErro");


// ============================================================
// CARREGAR USUÁRIO LEMBRADO
// ============================================================

const usuarioSalvo =
    localStorage.getItem("mainflow_usuario");

if (usuarioSalvo) {

    campoUsuario.value = usuarioSalvo;

    lembrarMe.checked = true;

}


// ============================================================
// LOGIN
// ============================================================

btnEntrar.addEventListener("click", async () => {

    const login =
        campoUsuario.value.trim();

    const senha =
        campoSenha.value;


    // --------------------------------------------------------
    // LIMPA MENSAGEM DE ERRO ANTERIOR
    // --------------------------------------------------------

    mensagemErro.textContent = "";


    // --------------------------------------------------------
    // VALIDA USUÁRIO E SENHA
    // --------------------------------------------------------

    if (!login || !senha) {

        mensagemErro.textContent =
            "⚠ Preencha o usuário e a senha.";

        return;
    }


    // --------------------------------------------------------
    // LEMBRAR-SE DE MIM
    // --------------------------------------------------------

    if (lembrarMe.checked) {

        localStorage.setItem(
            "mainflow_usuario",
            login
        );

    } else {

        localStorage.removeItem(
            "mainflow_usuario"
        );

    }


    // --------------------------------------------------------
    // DESABILITA BOTÃO
    // --------------------------------------------------------

    btnEntrar.disabled = true;

    btnEntrar.textContent =
        "Entrando...";


    // --------------------------------------------------------
    // ENVIA LOGIN PARA O MAINFLOW
    // --------------------------------------------------------

    try {

        const resposta =
            await fetch(
                "http://localhost:8080/api/login",
                {
                    method: "POST",

                    headers: {
                        "Content-Type":
                            "application/json"
                    },

                    body: JSON.stringify({

                        login: login,

                        senha: senha

                    })
                }
            );


        const dados =
            await resposta.json();


        // ----------------------------------------------------
        // LOGIN INCORRETO
        // ----------------------------------------------------

        if (!resposta.ok) {

            mensagemErro.textContent =
                "⚠ Usuário ou senha incorretos.";

            return;
        }


        // ----------------------------------------------------
        // LOGIN REALIZADO COM SUCESSO
        // ----------------------------------------------------

        if (dados.sucesso) {

    // Inicia a animação de saída
    document.querySelector(".Tela").classList.add("saindo");

    // Aguarda a animação terminar
    setTimeout(() => {
        window.location.href = "Ocorrencia.html";
    }, 700);
}
    }
    catch (erro) {

        console.error(erro);

        mensagemErro.textContent =
            "⚠ Não foi possível conectar ao servidor do MainFlow.";

    }
    finally {

        btnEntrar.disabled = false;

        btnEntrar.textContent =
            "Entrar";

    }

});