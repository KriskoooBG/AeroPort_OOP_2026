# AeroPort

`AeroPort` е конзолно C++ приложение за управление на летищен бизнес.  
Поддържа:

- управление на потребители с роли
- управление на инфраструктура на летището
- управление на авиокомпании, самолети и полети
- покупка и обработка на билети
- диспечерски операции по пистите
- запазване и зареждане на състоянието

## Основни роли

- `Administrator`
  Управлява инфраструктурата, авиокомпаниите, самолетите, полетите, времето и отчетите.

- `Dispatcher`
  Управлява движението по пистите, назначаването на полети и освобождаването на писти.

- `Passenger`
  Добавя средства, разглежда полети, купува билети, добавя багаж и отменя билети.

## Основни функционалности

- строене на писти и хангари
- регистриране на авиокомпании
- покупка и клониране на самолети
- планиране на полети
- купуване на `Standard`, `LastMinute` и `VIP` билети
- назначаване на полети към писти
- симулация на лошо време чрез `set-weather STORM`
- финансови справки и одити
- автоматично `save` при изход и `load` при стартиране

## Използвани design patterns

- `Singleton`  
  `AeroPortSystem` е единствената централна система.

- `Factory`  
  `AircraftFactory` създава самолети според типа им.

- `Builder`  
  `RunwayBuilder` създава писти с различни модули като `ILS`, `VIP`, `HeavyDuty`.

- `Prototype`  
  Самолетите се клонират чрез `cloneWithId()`.

- `Polymorphism`  
  Използва се при `User` и `Aircraft`.

## Структура на проекта

- `AeroPortSystem.*`  
  Главна бизнес логика и управление на състоянието.

- `CommandProcessor.*`  
  Парсване и изпълнение на конзолни команди.

- `User`, `Passenger`, `Dispatcher`, `Administrator`

- `Aircraft`, `PassengerPlane`, `CargoPlane`, `PrivateJet`

- `Runway`, `RunwayBuilder`, `Hangar`, `Airline`, `Flight`, `Ticket`

- `Enums.*`, `Validation.*`

## Стартиране във Visual Studio 2022

## Стартиране на програмата

При стартиране системата автоматично опитва:

- `load` от `aeroport_data.bin`
- създава default administrator:
  `username: admin`
  `password: admin`

При изход с `exit` системата автоматично извиква `save`.

## Формат на работа

Програмата работи изцяло чрез конзолни команди.

Пример:

```text
login admin admin
build-runway R1 3500 ILS HeavyDuty
register-airline SkyTeam 150000
buy-aircraft SkyTeam PassengerPlane AirbusA320 150
schedule-flight SK101 1 London 50
logout
register Ivan pass123 Passenger
login Ivan pass123
add-funds 300
book-ticket SK101 Standard
my-tickets
exit
```

## Команди по роли

### Общи команди

- `login [username] [password]`
- `register [username] [password] [Passenger|Dispatcher]`
- `help`
- `view-profile`
- `logout`
- `save`
- `load`
- `exit`

### Administrator

- `build-runway [id] [length] [ILS] [VIP] [HeavyDuty]`
- `build-hangar [id] [capacity] [repair_fee]`
- `close-runway [runway_id]`
- `set-weather [SUNNY|STORM]`
- `register-airline [name] [initial_capital]`
- `buy-aircraft [airline_name] [type] [model] [capacity_or_tonnage]`
- `clone-aircraft [aircraft_id] [count]`
- `send-to-hangar [aircraft_id] [hangar_id]`
- `retrieve-from-hangar [aircraft_id]`
- `schedule-flight [flight_id] [aircraft_id] [destination] [base_price]`
- `cancel-flight [flight_id]`
- `flight-revenue [flight_id]`
- `list-fleet [airline_name]`
- `airport-report`
- `audit-airline [airline_name]`

### Dispatcher

- `list-airspace`
- `list-runways`
- `assign-runway [flight_id] [runway_id]`
- `delay-flight [flight_id] [reason]`
- `free-runway [runway_id]`
- `undo`

### Passenger

- `add-funds [amount]`
- `list-flights [destination]`
- `filter-flights [max_price]`
- `book-ticket [flight_id] [Standard|LastMinute|VIP]`
- `upgrade-ticket [flight_id] [new_ticket_type]`
- `add-baggage [flight_id] [weight]`
- `cancel-ticket [flight_id]`
- `my-tickets`

## Правила на билетите

- `Standard`
  Цена = базова цена, билетът подлежи на отказ.

- `LastMinute`
  Цена = 50% от базовата цена, билетът не подлежи на отказ.

- `VIP`
  Цена = 2x базова цена, включва 20 кг багаж, билетът подлежи на отказ.

## Правила на самолетите

- `PassengerPlane`
  Минимална писта: `2000m`
  Health спад: `35%`
  Такса: `10%` от продадените билети

- `CargoPlane`
  Минимална писта: `3000m` + `HeavyDuty`
  Health спад: `45%`
  Такса: `15 EUR` на тон товароносимост

- `PrivateJet`
  Минимална писта: `1000m` + `VIP`
  Health спад: `25%`
  Такса: `5000 EUR`

## Save / Load

Системата пази данните във файла:

```
aeroport_data.bin
```

Този файл съдържа:

- потребители
- авиокомпании
- самолети
- полети
- писти
- хангари
- билети
- финансово състояние
