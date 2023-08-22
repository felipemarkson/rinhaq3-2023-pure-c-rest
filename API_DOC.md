# API Documentation

## Endpoints

The API has 3 endpoints. The endpoint `/pessoas` can receive a `GET` method with the
parameter `t` as a search token (case-insensitive). This endpoint also can receive a `POST`
method to create a new resource defined by the body. The endpoint `/pessoas/[:id]` can 
receive a `GET` to search by a specific `id`. The endpoint `/contagem-pessoas` returns 
the number of resources created.

- `POST /pessoas` – Resource creation.
- `GET /pessoas/[:id]` – Resource search by id.
- `GET /pessoas?t=[:token]` – Resource search by token in the fields `apelido`, `nome`, `stack`.
- `GET /contagem-pessoas` – Count of resources created.

Here is a sample of a valid `POST /pessoas` body:

```js
{
    "apelido": "jao",
    "nome": "João Silva",
    "nascimento": "1964-06-27",
    "stack": [
        "PHP",
        "C#",
        "NoSQL"
    ]
}
```
The header of the response will be something like:

```
< HTTP/1.1 201 Created
< Date: Tue, 22 Aug 2023 20:29:03 GMT
< Location: /pessoas/b00928bb-1ff6-4ddf-8f31-1d8712d70a6e
< Content-Length: 0
```

Here is a sample of the body response from a valid `GET /pessoas/589d12ca-a689-40c1-8fc3-1d7fd10e18d0`:

```js
{
	"id": "589d12ca-a689-40c1-8fc3-1d7fd10e18d0",
	"apelido": "catita",
	"nome": "Catia Hermano",
	"nascimento": "2003-01-01",
	"stack": [
		"C#",
		"Python",
		"JS"
	]
}
```

Here is a sample of the body response from a `GET /pessoas?t=c#` :

```js
[
    {
        "id": "589d12ca-a689-40c1-8fc3-1d7fd10e18d0",
        "apelido": "catita",
        "nome": "Catia Hermano",
        "nascimento": "2003-01-01",
        "stack": [
            "C#",
            "Python",
            "JS"
	    ]
    },
    {
        "apelido": "jao",
        "nome": "João Silva",
        "nascimento": "1964-06-27",
        "stack": [
            "PHP",
            "C#",
            "NoSQL"
        ]
    }
]
```

Here is a sample of the body response from a `GET /contagem-pessoas`

```js
2
```