@echo off
    mkdir ".\solucoesAIO" 2>nul
    echo.NOME_SOLU^|VAL_SOLUCAO_INICIAL^|QTD_TROCAS_SOL_INCIAL^|VAL_SOLUCAO_POS_REFI^|QTD_TROCAS_POS_REFI^|QTD_ITERACOES^|HOUVE_MELHORA^|TEMPO_HC^|TEMPO_REF^|TEMPO_TOTAL >> "solucoesAIO\result.csv"

for %%d in (8 15 25 50 100 200 ) do (

    for %%i in (./instances/%%djobs/*) do (
        echo | set /p="%%~ni|" >> "solucoesAIO\result.csv"
        loader < ./instances/%%djobs/%%i >> "solucoesAIO\result.csv"
    )
)