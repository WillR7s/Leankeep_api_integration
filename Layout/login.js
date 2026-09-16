const btnEntrar = document.getElementById("btnEntrar");

const campoUsuario = document.getElementById("usuario");
const campoSenha = document.getElementById("senha");
const lembrarMe = document.getElementById("lembrarMe");

const mensagemErro = document.getElementById("mensagemErro");


// ============================================================
// CARREGAR USUÁRIO LEMBRADO
// ============================================================

const usuarioSalvo = localStorage.getItem("mainflow_usuario");

if (usuarioSalvo) {

    campoUsuario.value = usuarioSalvo;

    lembrarMe.checked = true;

}


// ============================================================
// LOGIN
// ============================================================

btnEntrar.addEventListener("click", async () => {

    const login = campoUsuario.value.trim();
    const senha = campoSenha.value;


    // --------------------------------------------------------
    // LIMPA MENSAGEM DE ERRO
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

    btnEntrar.textContent = "Entrando...";


    // --------------------------------------------------------
    // ENVIA LOGIN PARA O MAINFLOW
    // --------------------------------------------------------

    try {

        const resposta = await fetch(
            "http://localhost:8080/api/login",
            {
                method: "POST",

                headers: {
                    "Content-Type": "application/json"
                },

                body: JSON.stringify({
                    login: login,
                    senha: senha
                })
            }
        );


        // ----------------------------------------------------
        // VERIFICA SE O SERVIDOR RESPONDEU
        // ----------------------------------------------------

        if (!resposta.ok) {

            mensagemErro.textContent =
                "⚠ Usuário ou senha incorretos.";

            return;
        }


        // ----------------------------------------------------
        // CONVERTE RESPOSTA PARA JSON
        // ----------------------------------------------------

        const dados = await resposta.json();

        console.log("Resposta do MainFlow:", dados);


        // ----------------------------------------------------
        // LOGIN REALIZADO COM SUCESSO
        // ----------------------------------------------------

        if (dados.sucesso === true) {

            // Mensagem opcional no console
            console.log("Login realizado com sucesso.");


            // ------------------------------------------------
            // ANIMAÇÃO DE SAÍDA
            // ------------------------------------------------

            const tela = document.querySelector(".Tela");

            if (tela) {
                tela.classList.add("saindo");
            }


            // ------------------------------------------------
            // VAI PARA A TELA DE OCORRÊNCIAS
            // ------------------------------------------------

            setTimeout(() => {

                window.location.href = "Ocorrencia.html";

            }, 700);


            return;
        }


        // ----------------------------------------------------
        // CASO O MAINFlow RESPONDA SEM SUCESSO
        // ----------------------------------------------------

        mensagemErro.textContent =
            "⚠ Usuário ou senha incorretos.";

    }


    // ========================================================
    // ERRO DE CONEXÃO
    // ========================================================

    catch (erro) {

        console.error(
            "Erro ao conectar ao MainFlow:",
            erro
        );

        mensagemErro.textContent =
            "⚠ Não foi possível conectar ao servidor do MainFlow.";

    }


    // ========================================================
    // RESTAURA BOTÃO
    // ========================================================

    finally {

        btnEntrar.disabled = false;

        btnEntrar.textContent = "Entrar";

    }

});