# ⚙️ Ligações Elétricas (Diagrama por Texto)
## 🔌 Alimentação AC (220V)
- Fase (L): conectada aos relés (COM dos SSR)
- Neutro (N): conectado diretamente ao fio neutro de cada válvula e bomba

## 🔧 Relé 1 (pino 18 do ESP32):
- COM → fase 220V
- NO → todos os fios de abertura das válvulas da solução e fechamento das válvulas dos peixes

## 🔧 Relé 2 (pino 19 do ESP32):
- COM → fase 220V
- NO → todos os fios de fechamento das válvulas da solução e abertura das válvulas dos peixes

## 🔧 Relé 3 (pino 23 do ESP32):
- COM → fase 220V
- NO → ligado ao fio fase das 6 bombas da solução

## 🔧 Relé 4 (pino 25 do ESP32):
- COM → fase 220V
- NO → ligado ao fio fase da bomba dos peixes
