@echo off
for %%d in (8 15 25 50 100 200 ) do (
    mkdir ".\solucoes\%%djobs" 2>nul
    echo.NOME_SOLU^|VAL_SOLUCAO_INICIAL^|QTD_TROCAS_SOL_INCIAL^|VAL_SOLUCAO_POS_REFI^|QTD_TROCAS_POS_REFI^|QTD_ITERACOES^|HOUVE_MELHORA^|TEMPO_HC^|TEMPO_REF^|TEMPO_TOTAL >> "solucoes\%%djobs\result.csv"


    for %%i in (./instances/%%djobs/*) do (
        echo | set /p="%%~ni|" >> "solucoes\%%djobs\result.csv"
        loader < ./instances/%%djobs/%%i >> "solucoes\%%djobs\result.csv"
    )
)