# c-sockets

Este repositório contém código relacionado ao Trabalho 2 de INF1406 - Programação Distribuída e Concorrente.

## Execução

Para compilar e executar um servidor, rodar (para usuários Unix)

`gcc -o server server<numero do servidor desejado>.c`

E para executar: 

`./server`

Com o servidor inicializado, o cliente pode ser compilado rodando

`gcc -o client client<numero do cliente desejado>`

e executando

`./client localhost`

## Testes e medição de tempo

Dois clientes foram desenvolvidos:
1. `client1` mantém conexão aberta durante todas as 5 requisições, com intervalo de tempo de 1 segundo entre cada requisição
2. `client2` abre uma conexão para cada uma das 5 requisições, com intervalo de tempo de 1 segundo entre cada conexão e requisição

O `client1` foi desenvolvido para testar a execução do servidor1, que também mantém conexão aberta. Ao executar ambos, vemos que o servidor inicializa aguardando conexão, se conecta com o cliente, o cliente é desconectado e o servidor volta a aceitar novas conexões:

Output `server1`: 
```
Waiting for new connections...
server: got connection from ::1
server: client has disconnected
Waiting for new connections...
```

Output `client1` (16332 milisegundos): 
```
client: connecting to ::1
16332
```

O `client2` foi desenvolvido para testar a execução do `server2`, `server3` e `server4` (com problemas), que abre uma nova conexão a cada nova requisião. 

Ao executar `client2` e `server2`, vemos que o servidor inicializa aguardando conexão, se conecta com o cliente, atende a requisição, fecha a conexão e aguarda nova conexão. Isto se repete cinco vezes para cada iteração do loop em `client2`.

Output `server2`: 
```
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
```

Output `client2` (14785 milisegundos): 
```
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
14785
```

Ao executar `client2` e `server3`, vemos que o servidor inicializa aguardando conexão, se conecta com o cliente, cria um processo filho, atende a requisição, fecha a conexão, termina o processo, e aguarda nova conexão. Isto se repete cinco vezes para cada iteração do loop em `client2`.

Output `server3`: 
```
Waiting for new connections...
server: got connection from ::1
Waiting for new connections...
Process 4133 handling connection
server: got connection from ::1
Waiting for new connections...
Process 4134 handling connection
server: got connection from ::1
Waiting for new connections...
Process 4136 handling connection
server: got connection from ::1
Waiting for new connections...
Process 4137 handling connection
server: got connection from ::1
Waiting for new connections...
Process 4138 handling connection
```

Output `client2` (14785 milisegundos): 
```
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
client: connecting to ::1
25157
```
