#include <iostream>
#include "Menu.h"

int main() {
    int opcao;

    do {
        // Exibe o menu principal
        exibirMenuPrincipal();
        std::cout << "Selecione uma opção: ";
        std::cin >> opcao;

        // Trata a escolha do usuário com base na opção selecionada
        switch (opcao) {
            case 1:
                gerenciarLivros(); 
                break;
            case 2:
                gerenciarUsuarios(); 
                break;
            case 3:
                std::cout << "Encerrando o sistema...\n";
                break;
            default:
                std::cout << "Opção inválida! Tente novamente.\n";
                break;
        }
    } while (opcao != 3); 

    return 0;
}
