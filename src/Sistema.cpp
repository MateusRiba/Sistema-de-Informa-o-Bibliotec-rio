#include "Sistema.h"
#include "Usuario.h"
#include "Livro.h"
#include "LivroFisico.h"
#include "LivroDigital.h"
#include "Emprestimo.h"
#include "Administrador.h"
#include "LeitorComum.h"


#include <iostream> 
#include <fstream>       
#include <sstream>      

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

void Sistema::adicionarLivroFisico(LivroFisico* livroFisico) {
    livrosFisicos.push_back(livroFisico); 
    adicionarLivro(livroFisico);           // Adiciona ao vetor geral
}

void Sistema::adicionarLivroDigital(LivroDigital* livroDigital) {
    livrosDigitais.push_back(livroDigital); 
    adicionarLivro(livroDigital);           // Adiciona ao vetor geral
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

bool Sistema::removerLivroFisicoPorISBN(const std::string& isbn) {
    for (auto it = livrosFisicos.begin(); it != livrosFisicos.end(); ++it) {
        if ((*it)->getISBN() == isbn) {
            LivroFisico* livroFisico = *it;
            removerLivroPorISBN(isbn); // Remove do vetor geral e deleta o objeto
            livrosFisicos.erase(it);    // Remove do vetor específico
            return true;
        }
    }
    return false;
}

bool Sistema::removerLivroDigitalPorISBN(const std::string& isbn) {
    for (auto it = livrosDigitais.begin(); it != livrosDigitais.end(); ++it) {
        if ((*it)->getISBN() == isbn) {
            LivroDigital* livroDigital = *it;
            removerLivroPorISBN(isbn); // Remove do vetor geral e deleta o objeto
            livrosDigitais.erase(it);  // Remove do vetor específico
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

  //Cria e armazena um objeto Emprestimo, ligando um Usuario e um Livro.
  
bool Sistema::realizarEmprestimo(Usuario* u, Livro* l, const std::string& dataEmp, const std::string& dataDev) {

//logica para diminuir o estoque se o livro for fisico
    auto* livrofisico = dynamic_cast<LivroFisico*>(l); //Aqui estou tentando converter l para um livroFisico, nse não for possivel (então é um livroDigital)

    if (livrofisico) {
        int estoqueAtual = livrofisico->getQuantidadeEstoque();
        if (estoqueAtual <= 0) {
            
            std::cout << "Nao ha estoque disponivel para esse livro fisico!\n";
            return false; 

        } else {
            // Decrementa o estoque em 1
            livrofisico->diminuirEstoque();
            std::cout << "Emprestimo: Estoque decremetado (agora = " << livrofisico->getQuantidadeEstoque() << ")\n";
        }
    }


    // Cria um novo emprestimo "por valor" e adiciona ao array
    Emprestimo emp(u, l, dataEmp, dataDev);
    emprestimos.push_back(emp);

    std::cout << "Empréstimo realizado com sucesso!\n";
    return true;
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

//Autenticação de Usuario
Usuario* Sistema::autenticarUsuarioCPFSenha(const std::string& cpf, const std::string& senha) const {
    for(const Usuario* u : usuarios) {
        if(u->getCpf() == cpf && u->getSenha() == senha) {
            
            std::cout << "Autenticação bem-sucedida! Bem-vindo, " << u->getNome() << "!\n";
            return const_cast<Usuario*>(u); // ???
        }
    }
    std::cout << "Erro: CPF ou senha inválidos. Tente novamente.\n";
    return nullptr;
}

bool Sistema::isAdministrador(const Usuario* usuario) const {
    return dynamic_cast<const Administrador*>(usuario) != nullptr;
}

//Funções de Leitura de dados

bool Sistema::carregarUsuarios(const std::string& arquivoUsuarios) {
    std::ifstream arq(arquivoUsuarios);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir arquivo de usuários: " << arquivoUsuarios << "\n";
        return false;
    }

    std::string linha;
    while (std::getline(arq, linha)) {
        if (linha.empty()) continue;  //pula linhas vazias

        std::stringstream ss(linha);
        std::string tipo, nome, cpf, endereco, telefone, senha;

        //Lê cada campo separado por ';'
        if (!std::getline(ss, tipo, ';')) continue;
        if (!std::getline(ss, nome, ';')) continue;
        if (!std::getline(ss, cpf, ';')) continue;
        if (!std::getline(ss, endereco, ';')) continue;
        if (!std::getline(ss, telefone, ';')) continue;
        if (!std::getline(ss, senha, ';')) continue;

        Usuario* novoUsuario = nullptr;
        if (tipo == "Administrador") {
            //Cria Administrador
            //se passa `this` como ponteiro do Sistema
            novoUsuario = new Administrador(nome, cpf, endereco, telefone, senha, this);
        } else if (tipo == "LeitorComum") {
            // Cria Leitor Comum
            novoUsuario = new LeitorComum(nome, cpf, endereco, telefone, senha);
        } else {
            std::cerr << "Tipo de usuário inválido ou desconhecido: " << tipo << "\n";
            continue;
        }

        //Adiciona ao vetor
        this->adicionarUsuario(novoUsuario);
    }

    arq.close();
    return true;
}

bool Sistema::carregarLivrosFisicos(const std::string& arquivoFisicos) {
    std::ifstream arq(arquivoFisicos);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir arquivo de livros físicos: " << arquivoFisicos << "\n";
        return false;
    }

    std::string linha;
    while (std::getline(arq, linha)) {
        if (linha.empty()) continue;

        std::stringstream ss(linha);
        std::string titulo, autor, isbn, editora, tipoCapa;
        int anoPublicacao, valorDiaria, pesoGramas, quantidadeEstoque;

        //Le cada campo separado por ';'
        if (!std::getline(ss, titulo, ';')) continue;
        if (!std::getline(ss, autor, ';')) continue;
        if (!std::getline(ss, isbn, ';')) continue;

        // para ano (pois é int)
        std::string anoStr;
        if (!std::getline(ss, anoStr, ';')) continue;
        anoPublicacao = std::stoi(anoStr); //Converte para int

        if (!std::getline(ss, editora, ';')) continue;

        //para valorDiaria
        std::string valDiariaStr;
        if (!std::getline(ss, valDiariaStr, ';')) continue;
        valorDiaria = std::stoi(valDiariaStr); 

        //para pesoGramas
        std::string pesoStr;
        if (!std::getline(ss, pesoStr, ';')) continue;
        pesoGramas = std::stoi(pesoStr);

        if (!std::getline(ss, tipoCapa, ';')) continue;

        // quantidadeEstoque
        std::string estoqueStr;
        if (!std::getline(ss, estoqueStr, ';')) continue;
        quantidadeEstoque = std::stoi(estoqueStr);

        // Cria objeto
        LivroFisico* lf = new LivroFisico(
            titulo, autor, isbn, anoPublicacao,
            editora, valorDiaria, pesoGramas,
            tipoCapa, quantidadeEstoque
        );

        // Adiciona no sistema
        this->adicionarLivroFisico(lf);
    }

    arq.close();
    return true;
}

bool Sistema::carregarLivrosDigitais(const std::string& arquivoDigitais) {
    std::ifstream arq(arquivoDigitais);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir arquivo de livros digitais: " << arquivoDigitais << "\n";
        return false;
    }

    std::string linha;
    while (std::getline(arq, linha)) {
        if (linha.empty()) continue;

        std::stringstream ss(linha);
        std::string titulo, autor, isbn, editora, formatoArquivo;
        int anoPublicacao, valorDiaria, tamanhoArquivoKB, licencaDigital;

        if (!std::getline(ss, titulo, ';')) continue;
        if (!std::getline(ss, autor, ';')) continue;
        if (!std::getline(ss, isbn, ';')) continue;

        std::string anoStr;
        if (!std::getline(ss, anoStr, ';')) continue;
        anoPublicacao = std::stoi(anoStr);

        if (!std::getline(ss, editora, ';')) continue;

        std::string valDiariaStr;
        if (!std::getline(ss, valDiariaStr, ';')) continue;
        valorDiaria = std::stoi(valDiariaStr);

        //tamanhoArquivoKB
        std::string tamStr;
        if (!std::getline(ss, tamStr, ';')) continue;
        tamanhoArquivoKB = std::stoi(tamStr);

        if (!std::getline(ss, formatoArquivo, ';')) continue;

        //licencaDigital
        std::string licencaStr;
        if (!std::getline(ss, licencaStr, ';')) continue;
        licencaDigital = std::stoi(licencaStr);

        //Cria objeto
        LivroDigital* ld = new LivroDigital(
            titulo, autor, isbn, anoPublicacao, 
            editora, valorDiaria, tamanhoArquivoKB,
            formatoArquivo, licencaDigital
        );

        // Adiciona ao sistema
        this->adicionarLivroDigital(ld);
    }

    arq.close();
    return true;
}

bool Sistema::salvarUsuarios(const std::string& arquivoUsuarios) const {
    std::ofstream arq(arquivoUsuarios);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir " << arquivoUsuarios << " para escrita.\n";
        return false;
    }

    for (Usuario* u : usuarios) {
        // Verificar se é Administrador
        if (dynamic_cast<Administrador*>(u)) {
            arq << "Administrador;";
        } else {
            arq << "LeitorComum;";
        }
        // Campos: nome;cpf;endereco;telefone;senha
        arq << u->getNome() << ";"
            << u->getCpf() << ";"
            << u->getEndereco() << ";"
            << u->getTelefone() << ";"
            << u->getSenha() << "\n";
    }

    arq.close();
    return true;
}

bool Sistema::salvarLivrosFisicos(const std::string& arquivoFisicos) const {
    std::ofstream arq(arquivoFisicos);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir " << arquivoFisicos << " para escrita.\n";
        return false;
    }

    for (LivroFisico* lf : livrosFisicos) {
        // campos: titulo;autor;isbn;anoPublicacao;editora;valorDiaria;pesoGramas;tipoCapa;quantidadeEstoque
        arq << lf->getTitulo() << ";"
            << lf->getAutor() << ";"
            << lf->getISBN() << ";"
            << lf->getAnoPublicacao() << ";"
            << lf->getEditora() << ";"
            << lf->getValorDiaria() << ";"
            << lf->getPesoGrama() << ";"
            << lf->getTipoCapa() << ";"
            << lf->getQuantidadeEstoque() << "\n";
    }

    arq.close();
    return true;
}

bool Sistema::salvarLivrosDigitais(const std::string& arquivoDigitais) const {
    std::ofstream arq(arquivoDigitais);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir " << arquivoDigitais << " para escrita.\n";
        return false;
    }

    for (LivroDigital* ld : livrosDigitais) {
        // Campos: titulo;autor;isbn;anoPublicacao;editora;valorDiaria;tamanhoArquivoKB;formatoArquivo;licencaDigital
        arq << ld->getTitulo() << ";"
            << ld->getAutor() << ";"
            << ld->getISBN() << ";"
            << ld->getAnoPublicacao() << ";"
            << ld->getEditora() << ";"
            << ld->getValorDiaria() << ";"
            << ld->getTamanhoArquivoKB() << ";"
            << ld->getFormatoArquivo() << ";"
            << ld->getLicensaDigital() << "\n";
    }

    arq.close();
    return true;
}


//Exibir listagens:
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

void Sistema::listarLivrosFisicos() const {
    std::cout << "=== Lista de Livros Físicos ===\n";
    for (const LivroFisico* lf : livrosFisicos) {
        lf->exibir();
        std::cout << std::endl;
    }
}

void Sistema::listarLivrosDigitais() const {
    std::cout << "=== Lista de Livros Digitais ===\n";
    for (const LivroDigital* ld : livrosDigitais) {
        ld->exibir();
        std::cout << std::endl;
    }
}


void Sistema::listarEmprestimos() const {
    std::cout << "=== Lista de Emprestimos ===\n";
    for (const Emprestimo& e : emprestimos) {
        e.exibirEmprestimo();
    }
}
