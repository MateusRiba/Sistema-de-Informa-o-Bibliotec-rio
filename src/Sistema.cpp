#include "Sistema.h"
#include "Usuario.h"
#include "Livro.h"
#include "LivroFisico.h"
#include "LivroDigital.h"
#include "Emprestimo.h"

#include <iostream> 

//O destrutor precisa destruir todos os usuarios e livros criados
Sistema::~Sistema() {
    // alocação feita com new
    for (Usuario* u : usuarios) {
    delete u;
    }
    for (Livro* l : livros) {
    delete l;
    }
    
}

void Sistema::adicionarUsuario(Usuario* usuario) {
    usuarios.push_back(usuario);
}

void Sistema::adicionarLivro(Livro* livro) {
    livros.push_back(livro);
}

bool Sistema::removerUsuarioPorCPF(const std::string& cpf) {
    for (auto it = usuarios.begin(); it != usuarios.end(); ++it) { //iterador percorre do inicio do array até o fim dela.
        if ((*it)->getCpf() == cpf) {
            delete *it;  //deleta o ponteiro
            usuarios.erase(it);  //apagga da lista
            return true;
        }
    }
    return false;
}

bool Sistema::removerUsuarioPorNome(const std::string& nome) {
    for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
        if ((*it)->getNome() == nome) {
            delete *it;
            usuarios.erase(it);
            return true;
        }
    }
    return false;
}

bool Sistema::removerLivroPorISBN(const std::string& isbn) {
    for (auto it = livros.begin(); it != livros.end(); ++it) {
        if ((*it)->getISBN() == isbn) {
            delete *it;
            livros.erase(it);
            return true;
        }
    }
    return false;
}

bool Sistema::removerLivroPorTitulo(const std::string& titulo) {
    for (auto it = livros.begin(); it != livros.end(); ++it) {
        if ((*it)->getTitulo() == titulo) {
            delete *it;
            livros.erase(it);
            return true;
        }
    }
    return false;
}

Usuario* Sistema::buscarUsuarioPorCPF(const std::string& cpf) const {
    for (Usuario* u : usuarios) {  //Loop é chamado de maneira diferente pois não precisa de um iterador (já que não usa o metodo erase)
        if (u->getCpf() == cpf) {
            return u;
        }
    }
    return nullptr; 
}

Usuario* Sistema::buscarUsuarioPorNome(const std::string& nome) const {
    for (Usuario* u : usuarios) {  
        if (u->getNome() == nome) {
            return u;
        }
    }
    return nullptr; 
}

Livro* Sistema::buscarLivroPorISBN(const std::string& isbn) const {
    for (Livro* l : livros) {
        if (l->getISBN() == isbn) {
            return l;
        }
    }
    return nullptr; 
}

Livro* Sistema::buscarLivroPorTitulo(const std::string& titulo) const {
    for (Livro* l : livros) {
        if (l->getISBN() == titulo) {
            return l;
        }
    }
    return nullptr; 
}

/*
  Cria e armazena um objeto Emprestimo, ligando um Usuario e um Livro.
  - Decrementa estoque se for LivroFisico, por exemplo (opcional).
*/
void Sistema::realizarEmprestimo(Usuario* u, Livro* l, const std::string& dataEmp, const std::string& dataDev) {

//logica para diminuir o estoque se o livro for fisico
    auto* livrofisico = dynamic_cast<LivroFisico*>(l); //Aqui estou tentando converter l para um livroFisico, nse não for possivel (então é um livroDigital)

    if (livrofisico) {
        int estoqueAtual = livrofisico->getQuantidadeEstoque();
        if (estoqueAtual <= 0) {
            
            std::cout << "Nao ha estoque disponivel para esse livro fisico!\n";
            return; 

        } else {
            // Decrementa o estoque em 1
            livrofisico->diminuirEstoque();
            std::cout << "Emprestimo: Estoque decremetado (agora = " << livrofisico->getQuantidadeEstoque() << ")\n";
        }
    }


    // Cria um novo emprestimo "por valor" e adiciona ao array
    Emprestimo emp(u, l, dataEmp, dataDev);
    emprestimos.push_back(emp);
}

/*
  Procura no array de Emprestimos qual corresponde ao CPF e ISBN, ou Nome e Titulo 
  e chama finalizarEmprestimo(dataRealDevolucao).
*/
bool Sistema::encerrarEmprestimoCpfIsbn(const std::string& cpfUsuario, const std::string& isbnLivro, const std::string& dataRealDevolucao) 
{
    for (Emprestimo &e : emprestimos) {
        //verifica se e.usuario->getCpf() == cpfUsuario e e.livro->getISBN() == isbnLivro
        if (e.isFinalizado() == false &&
            e.getUsuario()->getCpf() == cpfUsuario &&
            e.getLivro()->getISBN() == isbnLivro) 
        {
            e.finalizarEmprestimo(dataRealDevolucao);
            
            if (LivroFisico* lf = dynamic_cast<LivroFisico*>(e.getLivro())) {
                lf->aumentarEstoque();  // ou lf->diminuirEstoque(-1); etc.
                std::cout << "Estoque incrementado (agora = " << lf->getQuantidadeEstoque() << std::endl;
            }

            return true;
        }
    }
    std::cout << "Empréstimo não encontrado ou já finalizado.\n";
    return false;
}

bool Sistema::encerrarEmprestimoNomeTitulo(const std::string& nomeUsuario, const std::string& tituloLivro, const std::string& dataRealDevolucao) 
{
    for (Emprestimo &e : emprestimos) {
        //verifica se e.usuario->getCpf() == cpfUsuario e e.livro->getISBN() == isbnLivro
        if (e.isFinalizado() == false &&
            e.getUsuario()->getNome() == nomeUsuario &&
            e.getLivro()->getTitulo() == tituloLivro) 
        {
            e.finalizarEmprestimo(dataRealDevolucao);
            
            if (LivroFisico* lf = dynamic_cast<LivroFisico*>(e.getLivro())) {
                lf->aumentarEstoque();  // ou lf->diminuirEstoque(-1); etc.
                std::cout << "Estoque incrementado (agora = " << lf->getQuantidadeEstoque() << std::endl;
            }
            
            return true;
        }
    }
    std::cout << "Empréstimo não encontrado ou já finalizado.\n";
    return false;
}

//Metodos de Edição
//Usuario
bool Sistema::atualizarUsuario(const std::string& cpf,const std::string& novoNome,const std::string& novoEndereco,const std::string& novoTelefone, const std::string& novaSenha)
{
    for (Usuario* u : usuarios) {
        if (u->getCpf() == cpf) {
            //setters do usuario:
            u->setNome(novoNome);
            u->setEndereco(novoEndereco);
            u->setTelefone(novoTelefone);
            u->setSenha(novaSenha);
            return true;
        }
    }
    return false; // Não encontrado
}

//Livro
bool Sistema::atualizarLivro(const std::string& isbn,
                             const std::string& novoTitulo,
                             const std::string& novoAutor,
                             const std::string& novaEditora,
                             double novoValorDiaria,
                             int novoPesoGrama,
                             const std::string& novaCapa,
                             int novoEstoque,
                             int novoTamanhoArquivo,
                             const std::string& novoFormatoArquivo,
                             int novaLicensaDigital)
{
    for (Livro* l : livros) {
        if (l->getISBN() == isbn) {
            // Atributos comuns da classe base
            l->setTitulo(novoTitulo);
            l->setAutor(novoAutor);
            l->setEditora(novaEditora);
            l->setValorDiaria(novoValorDiaria);

            // Agora, testa se é um LivroFisico
            if (auto* lf = dynamic_cast<LivroFisico*>(l)) {
                // Se novoPesoGrama != -1, então set
                if (novoPesoGrama != -1) {
                    lf->setPesoGrama(novoPesoGrama);
                }
                if (!novaCapa.empty()) {
                    lf->setTipoCapa(novaCapa);
                }
                if (novoEstoque != -1) {
                    lf->setQuantidadeEstoque(novoEstoque);
                }
            }
            // Se for um LivroDigital
            else if (auto* ld = dynamic_cast<LivroDigital*>(l)) {
                if (novoTamanhoArquivo != -1) {
                    ld->setTamanhoArquivoKB(novoTamanhoArquivo);
                }
                if (!novoFormatoArquivo.empty()) {
                    ld->setFormatoArquivo(novoFormatoArquivo);
                }
                if(novaLicensaDigital!= -1) {
                    ld->setLicensaDigital(novaLicensaDigital);
                }
            }
            return true; // Atualizado com sucesso
        }
    }
    return false; // Não encontrou
}

//Emprestimo
bool Sistema::atualizarEmprestimo(const std::string& cpfUsuario,const std::string& isbnLivro,const std::string& novaDataEmp,const std::string& novaDataDev, int novoCusto) // <-- Recebe custo
{
    for (Emprestimo &e : emprestimos) {
        if (!e.isFinalizado() &&
            e.getUsuario()->getCpf() == cpfUsuario &&
            e.getLivro()->getISBN() == isbnLivro) 
        {
            e.setDataEmprestimo(novaDataEmp);
            e.setDataDevolucao(novaDataDev);
            e.setCusto(novoCusto); 
            return true;
        }
    }
    return false;
}

// Exibir listagens:
void Sistema::listarUsuarios() const {
    std::cout << "=== Lista de Usuarios ===\n";
    for (Usuario* u : usuarios) {
        u->exibir(); 
    }
}

void Sistema::listarLivros() const {
    std::cout << "=== Lista de Livros ===\n";
    for (Livro* l : livros) {
        l->exibir();
        std::cout << std::endl;
    }
}

void Sistema::listarEmprestimos() const {
    std::cout << "=== Lista de Emprestimos ===\n";
    for (const Emprestimo& e : emprestimos) {
        e.exibirEmprestimo();
    }
}
