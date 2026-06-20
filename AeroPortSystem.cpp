#include "AeroPortSystem.h"

#include "Administrator.h"
#include "AircraftFactory.h"
#include "Dispatcher.h"
#include "Passenger.h"
#include "RunwayBuilder.h"

#include <cstddef>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {
std::string formatMoney(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}

std::string formatNumber(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}
}

AeroPortSystem& AeroPortSystem::instance() {
    static AeroPortSystem system;
    return system;
}

std::string AeroPortSystem::setWeather(Weather weather) {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can set weather.";

    std::ostringstream out;
    out << "[ALERT] Global Weather Conditions changed to " << toString(weather) << "!";

    if (weather == Weather::Storm) {
        out << "\n[Observer] Verifying airport infrastructure for safe landing conditions...";

        bool anyIls = false;
        std::string inactiveIlsRunway;
        for (const auto& kv : runways) {
            if (kv.second.supportsIls() && kv.second.getStatus() != RunwayStatus::Maintenance) {
                anyIls = true;
                break;
            }
            if (kv.second.supportsIls() && kv.second.getStatus() == RunwayStatus::Maintenance && inactiveIlsRunway.empty()) {
                inactiveIlsRunway = kv.second.getId();
            }
        }

        if (!anyIls) {
            out << "\n[System] WARNING: No active runways with ILS available!";
            if (!inactiveIlsRunway.empty()) {
                out << " (" << inactiveIlsRunway << " is currently in Maintenance).";
            }
        }

        for (auto& kv : flights) {
            Flight& f = kv.second;
            if (f.getStatus() == FlightStatus::Departed || f.getStatus() == FlightStatus::Cancelled) {
                continue;
            }

            bool hasSafeRunway = false;
            const auto assignedRunwayId = f.getAssignedRunwayId();
            if (assignedRunwayId.has_value()) {
                const auto runwayIt = runways.find(assignedRunwayId.value());
                hasSafeRunway = runwayIt != runways.end()
                    && runwayIt->second.supportsIls()
                    && runwayIt->second.getStatus() != RunwayStatus::Maintenance;
            } else {
                hasSafeRunway = anyIls;
            }

            if (hasSafeRunway) {
                continue;
            }

            double refundedTotal = 0.0;
            std::string refundedPassenger;
            int refundedTickets = 0;
            for (int ti : f.getTicketIndexes()) {
                if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
                    Ticket& t = tickets[ti];
                    if (!t.isActive()) continue;

                    const double amount = t.getPaidAmount();
                    const auto userIt = users.find(t.getOwnerName());
                    if (userIt != users.end() && userIt->second->getRole() == Role::Passenger) {
                        userIt->second->passengerDeposit(amount);
                        refundedPassenger = t.getOwnerName();
                    }

                    refundedTotal += amount;
                    ++refundedTickets;
                    t.cancel();
                }
            }

            if (assignedRunwayId.has_value()) {
                const auto runwayIt = runways.find(assignedRunwayId.value());
                if (runwayIt != runways.end()) {
                    runwayIt->second.free();
                }
                f.clearRunway();
            }

            f.setStatus(FlightStatus::Cancelled);
            out << "\n[Observer] Flight " << f.getId() << " Cancelled due to severe weather!";
            if (refundedTotal > 0.0) {
                out << "\n[System] Auto-refunding " << std::fixed << std::setprecision(2)
                    << refundedTotal << " EUR";
                if (refundedTickets == 1 && !refundedPassenger.empty()) {
                    out << " to passenger " << refundedPassenger;
                } else {
                    out << " for flight " << f.getId();
                }
                out << "...";
            } else {
                out << "\n[System] Auto-refunding passengers for flight " << f.getId() << "...";
            }
        }
    }

    return out.str();
}

std::string AeroPortSystem::save(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return "[Error] Failed to open file for saving.";

    out << "AeroPort_v1" << '\n';
    out << nextAircraftId << '\n';
    out << std::fixed << std::setprecision(2) << airportBalance << '\n';
    out << '\n';

    out << users.size() << '\n';
    for (const auto& kv : users) {
        const auto& u = kv.second;
        out << u->getUsername() << '\n';
        out << u->getPassword() << '\n';
        out << int(u->getRole()) << '\n';
        if (u->getRole() == Role::Passenger) {
            out << u->getPassengerBalance() << '\n';
        }
    }

    out << airlines.size() << '\n';
    for (const auto& kv : airlines) {
        const auto& a = kv.second;
        out << a->getName() << '\n';
        out << std::fixed << std::setprecision(2) << a->getBalance() << '\n';
        const auto& aids = a->getAircraftIds();
        out << aids.size() << '\n';
        for (int id : aids) out << id << '\n';
        const auto& fids = a->getFlightIds();
        out << fids.size() << '\n';
        for (const auto& fid : fids) out << fid << '\n';
    }

    out << aircraft.size() << '\n';
    for (const auto& kv : aircraft) {
        const auto& ac = kv.second;
        out << ac->getId() << '\n';
        out << int(ac->getType()) << '\n';
        out << ac->getModel() << '\n';
        out << ac->getAirlineName() << '\n';
        out << ac->getHealth() << '\n';
        out << ac->getCapacityUnits() << '\n';
    }

    out << flights.size() << '\n';
    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        out << f.getId() << '\n';
        out << f.getAircraftId() << '\n';
        out << f.getAirlineName() << '\n';
        out << f.getDestination() << '\n';
        out << std::fixed << std::setprecision(2) << f.getBasePrice() << '\n';
        out << int(f.getStatus()) << '\n';
        if (f.getAssignedRunwayId().has_value()) out << f.getAssignedRunwayId().value() << '\n'; else out << "-" << '\n';
        const auto& tix = f.getTicketIndexes();
        out << tix.size() << '\n';
        for (int idx : tix) out << idx << '\n';
    }

    out << runways.size() << '\n';
    for (const auto& kv : runways) {
        const Runway& r = kv.second;
        out << r.getId() << '\n';
        out << r.getLength() << '\n';
        out << (r.supportsIls() ? 1 : 0) << '\n';
        out << (r.supportsVipTerminal() ? 1 : 0) << '\n';
        out << (r.supportsHeavyDuty() ? 1 : 0) << '\n';
        out << int(r.getStatus()) << '\n';
        if (r.getOccupiedAircraftId().has_value()) out << r.getOccupiedAircraftId().value() << '\n'; else out << -1 << '\n';
    }

    out << hangars.size() << '\n';
    for (const auto& kv : hangars) {
        const Hangar& h = kv.second;
        out << h.getId() << '\n';
        out << h.getCapacity() << '\n';
        out << std::fixed << std::setprecision(2) << h.getRepairFee() << '\n';
        const auto& aids = h.getAircraftIds();
        out << aids.size() << '\n';
        for (int id : aids) out << id << '\n';
    }

    out << tickets.size() << '\n';
    for (const auto& t : tickets) {
        out << t.getOwnerName() << '\n';
        out << t.getFlightId() << '\n';
        out << int(t.getType()) << '\n';
        out << std::fixed << std::setprecision(2) << t.getPaidAmount() << '\n';
        out << t.getBaggageKg() << '\n';
        out << (t.isActive() ? 1 : 0) << '\n';
    }

    out.close();
    return "[System] AeroPort application state successfully serialized and saved to '" + filename + "'.";
}

std::string AeroPortSystem::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return "[System] No save file found. Starting a fresh instance of AeroPort System.";

    std::string header;
    std::getline(in, header);
    if (header != "AeroPort_v1") {
        return "[System] Save file version mismatch or corrupt. Keeping current AeroPort System state.";
    }

    users.clear();
    airlines.clear();
    flights.clear();
    runways.clear();
    hangars.clear();
    aircraft.clear();
    tickets.clear();
    actionHistory.clear();

    in >> nextAircraftId;
    in >> airportBalance;
    in.ignore();
    std::getline(in, currentUsername);
    currentUsername.clear();

    size_t userCount = 0;
    in >> userCount;
    in.ignore();
    for (size_t i = 0; i < userCount; ++i) {
        std::string uname, pwd;
        std::getline(in, uname);
        std::getline(in, pwd);
        int roleInt = 0;
        in >> roleInt;
        Role role = static_cast<Role>(roleInt);
        in.ignore();
        if (role == Role::Passenger) {
            double bal = 0.0;
            in >> bal;
            in.ignore();
            users.emplace(uname, std::make_unique<Passenger>(uname, pwd, bal));
        } else if (role == Role::Dispatcher) {
            users.emplace(uname, std::make_unique<Dispatcher>(uname, pwd));
        } else if (role == Role::Administrator) {
            users.emplace(uname, std::make_unique<Administrator>());
        }
    }

    size_t airlineCount = 0;
    in >> airlineCount;
    in.ignore();
    for (size_t i = 0; i < airlineCount; ++i) {
        std::string name;
        std::getline(in, name);
        double bal = 0.0;
        in >> bal;
        Airline a(name, bal);
        size_t acCount = 0;
        in >> acCount;
        for (size_t j = 0; j < acCount; ++j) {
            int aid; in >> aid; a.addAircraft(aid);
        }
        size_t fCount = 0;
        in >> fCount;
        in.ignore();
        for (size_t j = 0; j < fCount; ++j) {
            std::string fid; std::getline(in, fid); a.addFlight(fid);
        }
        airlines.emplace(name, std::make_shared<Airline>(std::move(a)));
    }

    size_t acCount = 0; in >> acCount; in.ignore();
    for (size_t i = 0; i < acCount; ++i) {
        int id; in >> id; in.ignore();
        int typeInt; in >> typeInt; in.ignore();
        std::string model; std::getline(in, model);
        std::string airlineName; std::getline(in, airlineName);
        int health; in >> health; in.ignore();
        int cap; in >> cap; in.ignore();
        AircraftType type = static_cast<AircraftType>(typeInt);
        auto acPtr = AircraftFactory::create(id, type, model, airlineName, cap);
        if (acPtr) {
            acPtr->setHealth(health);
            aircraft.emplace(id, std::move(acPtr));
        }
    }

    size_t flightsCount = 0; in >> flightsCount; in.ignore();
    for (size_t i = 0; i < flightsCount; ++i) {
        std::string id; std::getline(in, id);
        int aircraftId; in >> aircraftId; in.ignore();
        std::string airlineName; std::getline(in, airlineName);
        std::string destination; std::getline(in, destination);
        double basePrice; in >> basePrice; in.ignore();
        int statusInt; in >> statusInt; in.ignore();
        std::string assigned; std::getline(in, assigned);
        int tcount; in >> tcount; in.ignore();
        Flight f(id, aircraftId, airlineName, destination, basePrice);
        f.setStatus(static_cast<FlightStatus>(statusInt));
        if (assigned != "-") f.assignRunway(assigned);
        for (int j = 0; j < tcount; ++j) { int idx; in >> idx; in.ignore(); f.addTicketIndex(idx); }
        flights.emplace(id, std::move(f));
    }

    size_t runwayCount = 0; in >> runwayCount; in.ignore();
    for (size_t i = 0; i < runwayCount; ++i) {
        std::string id; std::getline(in, id);
        int length; in >> length; in.ignore();
        int ils; in >> ils; in.ignore();
        int vip; in >> vip; in.ignore();
        int heavy; in >> heavy; in.ignore();
        int statusInt; in >> statusInt; in.ignore();
        int occ; in >> occ; in.ignore();
        Runway r(id, length, ils != 0, vip != 0, heavy != 0);
        if (occ != -1) r.occupy(occ);
        if (statusInt == static_cast<int>(RunwayStatus::Maintenance)) r.closeForMaintenance();
        runways.emplace(id, std::move(r));
    }

    size_t hangarCount = 0; in >> hangarCount; in.ignore();
    for (size_t i = 0; i < hangarCount; ++i) {
        std::string id; std::getline(in, id);
        int cap; in >> cap; in.ignore();
        double fee; in >> fee; in.ignore();
        Hangar h(id, cap, fee);
        size_t aCount = 0; in >> aCount; in.ignore();
        for (size_t j = 0; j < aCount; ++j) { int aid; in >> aid; in.ignore(); h.admitAircraft(aid); }
        hangars.emplace(id, std::move(h));
    }

    size_t ticketCount = 0; in >> ticketCount; in.ignore();
    for (size_t i = 0; i < ticketCount; ++i) {
        std::string owner; std::getline(in, owner);
        std::string fid; std::getline(in, fid);
        int typeInt; in >> typeInt; in.ignore();
        double paid; in >> paid; in.ignore();
        double baggage; in >> baggage; in.ignore();
        int active; in >> active; in.ignore();
        tickets.emplace_back(owner, fid, static_cast<TicketType>(typeInt), paid, baggage);
        if (!active) tickets.back().cancel();
    }

    in.close();
    return "[System] AeroPort application state successfully loaded from '" + filename + "'.";
}

AeroPortSystem::AeroPortSystem() {
    users.emplace("admin", std::make_shared<Administrator>());
}

std::string AeroPortSystem::registerUser(const std::string& username, const std::string& password, Role role) {
    if (role == Role::Administrator) {
        return "[Error] Registering another administrator is not allowed.";
    }
    if (userExists(username)) {
        return "[Error] User '" + username + "' already exists.";
    }

    if (role == Role::Passenger) {
        users.emplace(username, std::make_shared<Passenger>(username, password));
    } else if (role == Role::Dispatcher) {
        users.emplace(username, std::make_shared<Dispatcher>(username, password));
    }

    return "[System] User '" + username + "' registered successfully (Role: " + toString(role) + ").";
}

std::string AeroPortSystem::login(const std::string& username, const std::string& password) {
    const auto it = users.find(username);
    if (it == users.end() || !it->second->checkPassword(password)) {
        return "[Error] Invalid credentials. Please try again.";
    }

    currentUsername = username;
    return "[System] Successfully logged in. Role: " + toString(it->second->getRole()) + ".";
}

std::string AeroPortSystem::logout() {
    if (!hasCurrentUser()) {
        return "[Error] No user is currently logged in.";
    }

    currentUsername.clear();
    return "[System] Logged out successfully.";
}

std::string AeroPortSystem::help() const {
    auto user = currentUser();
    if (!user) {
        return "[System] Available commands: login, register, save, load, exit";
    }
    std::string out = "[System] Available commands for " + toString(user->getRole()) + ": ";
    const auto commands = user->availableCommands();
    for (std::size_t i = 0; i < commands.size(); ++i) {
        if (i > 0) out += ", ";
        out += commands[i];
    }
    return out;
}

std::string AeroPortSystem::viewProfile() const {
    auto user = currentUser();
    if (!user) {
        return "[Error] You must login first.";
    }
    return "[Profile] " + user->profileInfo();
}

bool AeroPortSystem::hasCurrentUser() const {
    return !currentUsername.empty() && users.find(currentUsername) != users.end();
}

std::shared_ptr<const User> AeroPortSystem::currentUser() const {
    auto it = users.find(currentUsername);
    if (it == users.end()) return nullptr;
    return it->second;
}

std::shared_ptr<User> AeroPortSystem::currentUser() {
    auto it = users.find(currentUsername);
    if (it == users.end()) return nullptr;
    return it->second;
}

std::string AeroPortSystem::buildRunway(
    const std::string& id,
    int length,
    bool hasIls,
    bool hasVipTerminal,
    bool hasHeavyDuty
) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can build runways.";
    }
    if (length <= 0) {
        return "[Error] Runway length must be positive.";
    }
    if (runways.find(id) != runways.end()) {
        return "[Error] Runway '" + id + "' already exists.";
    }

    Runway runway = RunwayBuilder()
        .withId(id)
        .withLength(length)
        .withIls(hasIls)
        .withVipTerminal(hasVipTerminal)
        .withHeavyDuty(hasHeavyDuty)
        .build();

    runways.emplace(id, runway);
    return "[System] Runway " + runway.info() + " built successfully.";
}

std::string AeroPortSystem::buildHangar(const std::string& id, int capacity, double repairFee) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can build hangars.";
    }
    if (capacity <= 0) {
        return "[Error] Hangar capacity must be positive.";
    }
    if (repairFee < 0.0) {
        return "[Error] Repair fee cannot be negative.";
    }
    if (hangars.find(id) != hangars.end()) {
        return "[Error] Hangar '" + id + "' already exists.";
    }

    hangars.emplace(id, Hangar(id, capacity, repairFee));

    std::ostringstream out;
    out << "[System] Hangar " << id << " built successfully (Capacity: "
        << capacity << ", Repair Fee: " << std::fixed << std::setprecision(2)
        << repairFee << " EUR).";
    return out.str();
}

std::string AeroPortSystem::closeRunway(const std::string& runwayId) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can close runways.";
    }

    auto runwayIt = runways.find(runwayId);
    if (runwayIt == runways.end()) {
        return "[Error] Runway '" + runwayId + "' does not exist.";
    }

    int returnedFlights = 0;
    for (auto& kv : flights) {
        Flight& flight = kv.second;
        const auto assignedRunway = flight.getAssignedRunwayId();
        if (assignedRunway.has_value() && assignedRunway.value() == runwayId) {
            flight.clearRunway();
            flight.setStatus(FlightStatus::Scheduled);
            ++returnedFlights;
        }
    }

    runwayIt->second.closeForMaintenance();

    std::ostringstream out;
    out << "[System] Runway " << runwayId << " is now closed. Status -> Maintenance.";
    if (returnedFlights > 0) {
        out << " Returned assigned flights to Scheduled: " << returnedFlights << ".";
    }
    return out.str();
}

std::string AeroPortSystem::registerAirline(const std::string& name, double initialCapital) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can register airlines.";
    }
    if (initialCapital < 0.0) {
        return "[Error] Initial capital cannot be negative.";
    }
    if (airlines.find(name) != airlines.end()) {
        return "[Error] Airline '" + name + "' already exists.";
    }

    airlines.emplace(name, std::make_shared<Airline>(name, initialCapital));

    std::ostringstream out;
    out << "[System] Airline '" << name << "' registered. Corporate Balance: "
        << std::fixed << std::setprecision(2) << initialCapital << " EUR.";
    return out.str();
}

std::string AeroPortSystem::buyAircraft(
    const std::string& airlineName,
    AircraftType type,
    const std::string& model,
    int capacityOrTonnage
) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can buy aircraft.";
    }

    auto airlineIt = airlines.find(airlineName);
    if (airlineIt == airlines.end()) {
        return "[Error] Airline '" + airlineName + "' does not exist.";
    }
    if (capacityOrTonnage <= 0) {
        return "[Error] Capacity/tonnage must be positive.";
    }

    constexpr double aircraftPrice = 10000.0;
    if (!airlineIt->second->withdraw(aircraftPrice)) {
        return "[Error] Airline '" + airlineName + "' has insufficient corporate balance.";
    }

    const int newId = nextAircraftId++;
    std::unique_ptr<Aircraft> newAircraft = AircraftFactory::create(newId, type, model, airlineName, capacityOrTonnage);
    airlineIt->second->addAircraft(newId);
    aircraft.emplace(newId, std::shared_ptr<Aircraft>(std::move(newAircraft)));

    std::ostringstream out;
    out << "[System] Purchased " << toString(type) << " (ID: " << newId << "). "
        << airlineName << " balance: " << std::fixed << std::setprecision(2)
        << airlineIt->second->getBalance() << " EUR.";
    return out.str();
}

std::string AeroPortSystem::cloneAircraft(int aircraftId, int count) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can clone aircraft.";
    }
    if (count <= 0) {
        return "[Error] Clone count must be positive.";
    }

    const auto aircraftIt = aircraft.find(aircraftId);
    if (aircraftIt == aircraft.end()) {
        return "[Error] Aircraft with ID " + std::to_string(aircraftId) + " does not exist.";
    }

    auto airline = findAirlineByAircraftId(aircraftId);
    if (!airline) {
        return "[Error] Aircraft owner airline was not found.";
    }

    constexpr double aircraftPrice = 10000.0;
    const double totalPrice = aircraftPrice * count;
    if (!airline->withdraw(totalPrice)) {
        return "[Error] Airline '" + airline->getName() + "' has insufficient corporate balance.";
    }

    std::string out = "[System] Aircraft ID: " + std::to_string(aircraftId) + " cloned. New aircraft IDs:";
    for (int i = 0; i < count; ++i) {
        const int newId = nextAircraftId++;
        auto clonePtr = aircraftIt->second->cloneWithId(newId);
        airline->addAircraft(newId);
        aircraft.emplace(newId, std::shared_ptr<Aircraft>(std::move(clonePtr)));
        out += " " + std::to_string(newId);
    }

    out += ". " + airline->getName() + " balance: " + formatMoney(airline->getBalance()) + " EUR.";
    return out;
}

std::string AeroPortSystem::scheduleFlight(
    const std::string& flightId,
    int aircraftId,
    const std::string& destination,
    double basePrice
) {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can schedule flights.";
    }
    if (flights.find(flightId) != flights.end()) {
        return "[Error] Flight '" + flightId + "' already exists.";
    }
    if (basePrice <= 0.0) {
        return "[Error] Base price must be positive.";
    }

    const auto aircraftIt = aircraft.find(aircraftId);
    if (aircraftIt == aircraft.end()) {
        return "[Error] Aircraft with ID " + std::to_string(aircraftId) + " does not exist.";
    }
    if (!aircraftIt->second->canFly()) {
        return "[Error] Aircraft health is below 20%. Send it to a hangar first.";
    }

    for (const auto& kv : flights) {
        const Flight& existingFlight = kv.second;
        if (existingFlight.getAircraftId() == aircraftId &&
            existingFlight.getStatus() != FlightStatus::Departed &&
            existingFlight.getStatus() != FlightStatus::Cancelled) {
            return "[Error] Aircraft is already assigned to active flight '" + existingFlight.getId() + "'.";
        }
    }

    const std::string airlineName = aircraftIt->second->getAirlineName();
    auto airlineIt = airlines.find(airlineName);
    if (airlineIt == airlines.end()) {
        return "[Error] Aircraft owner airline was not found.";
    }

    flights.emplace(flightId, Flight(flightId, aircraftId, airlineName, destination, basePrice));
    airlineIt->second->addFlight(flightId);

    return "[System] " + toString(aircraftIt->second->getType()) + " flight " + flightId
        + " to " + destination + " is now Scheduled.";
}

std::string AeroPortSystem::sendToHangar(int aircraftId, const std::string& hangarId) {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can send aircraft to hangar.";
    auto acIt = aircraft.find(aircraftId);
    if (acIt == aircraft.end()) return "[Error] Aircraft with ID " + std::to_string(aircraftId) + " does not exist.";
    for (const auto& kv : flights) {
        const Flight& flight = kv.second;
        if (flight.getAircraftId() == aircraftId &&
            flight.getStatus() != FlightStatus::Departed &&
            flight.getStatus() != FlightStatus::Cancelled) {
            return "[Error] Aircraft is assigned to active flight '" + flight.getId() + "' and cannot be sent to hangar.";
        }
    }
    auto hIt = hangars.find(hangarId);
    if (hIt == hangars.end()) return "[Error] Hangar '" + hangarId + "' does not exist.";
    Hangar& h = hIt->second;
    if (!h.hasFreeSlot()) return "[Error] Hangar " + hangarId + " is at maximum capacity (" + std::to_string(static_cast<int>(h.getAircraftIds().size())) + "/" + std::to_string(h.getCapacity()) + " slots occupied)!";
    if (h.containsAircraft(aircraftId)) return "[Error] Aircraft is already in the hangar.";

    auto airline = findAirlineByAircraftId(aircraftId);
    if (!airline) return "[Error] Airline for aircraft not found.";

    double fee = h.getRepairFee();
    DispatcherAction act;
    act.type = DispatcherAction::Type::HangarAdmit;
    act.runwayId = hangarId;
    act.aircraftId = aircraftId;
    act.prevAirlineBalance = airline->getBalance();

    if (!airline->withdraw(fee)) return "[Error] " + std::string("Airline '") + airline->getName() + " has insufficient corporate balance.";

    if (!h.admitAircraft(aircraftId)) return "[Error] Failed to admit aircraft to hangar.";

    actionHistory.push_back(act);

    std::ostringstream out;
    out << "[Success] Aircraft ID: " << aircraftId << " admitted to Hangar " << hangarId << ". " << std::fixed << std::setprecision(2) << fee << " EUR deducted from " << airline->getName() << " balance.";
    return out.str();
}

std::string AeroPortSystem::retrieveFromHangar(int aircraftId) {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can retrieve aircraft from hangar.";
    Hangar* found = nullptr;
    std::string hid;
    for (auto& kv : hangars) {
        if (kv.second.containsAircraft(aircraftId)) { found = &kv.second; hid = kv.first; break; }
    }
    if (!found) return "[Error] Aircraft ID: " + std::to_string(aircraftId) + " is not in any hangar.";

    auto acIt = aircraft.find(aircraftId);
    int prevHealth = 0;
    if (acIt != aircraft.end()) prevHealth = acIt->second->getHealth();

    DispatcherAction act;
    act.type = DispatcherAction::Type::HangarRetrieve;
    act.runwayId = hid;
    act.aircraftId = aircraftId;
    act.prevAircraftHealth = prevHealth;

    if (!found->releaseAircraft(aircraftId)) return "[Error] Failed to release aircraft from hangar.";
    if (acIt != aircraft.end()) {
        acIt->second->repair();
    }

    actionHistory.push_back(act);

    std::ostringstream out;
    out << "[Success] Aircraft ID: " << aircraftId << " retrieved from Hangar " << hid << ". Health restored to 100%.";
    return out.str();
}

std::string AeroPortSystem::cancelFlight(const std::string& flightId) {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can cancel flights.";
    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight '" + flightId + "' does not exist.";
    Flight& f = fit->second;
    if (f.getStatus() == FlightStatus::Departed) return "[Error] Flight '" + flightId + "' has already Departed. It cannot be cancelled.";

    double totalRefund = 0.0;
    int refundedCount = 0;
    for (int ti : f.getTicketIndexes()) {
        if (ti >=0 && ti < static_cast<int>(tickets.size())) {
            Ticket& t = tickets[ti];
            if (!t.isActive()) continue;
            double amount = t.getPaidAmount();
            const auto uit = users.find(t.getOwnerName());
            if (uit != users.end() && uit->second->getRole() == Role::Passenger) {
                uit->second->passengerDeposit(amount);
            }
            totalRefund += amount;
            refundedCount++;
            t.cancel();
        }
    }

    f.setStatus(FlightStatus::Cancelled);

    std::ostringstream out;
    out << "[System] Flight " << flightId << " cancelled. Auto-refunding " << refundedCount << " passengers (" << std::fixed << std::setprecision(2) << totalRefund << " EUR).";
    return out.str();
}

std::string AeroPortSystem::flightRevenue(const std::string& flightId) const {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can view flight revenue.";
    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight does not exist.";
    const Flight& f = fit->second;

    double totalTickets = 0.0;
    for (int ti : f.getTicketIndexes()) {
        if (ti >=0 && ti < static_cast<int>(tickets.size())) {
            const Ticket& t = tickets[ti];
            if (t.isActive()) totalTickets += t.getPaidAmount();
        }
    }

    auto acIt = aircraft.find(f.getAircraftId());
    if (acIt == aircraft.end()) return "[Error] Aircraft not found.";
    const Aircraft* ac = acIt->second.get();

    double airportFee = 0.0;
    if (f.getStatus() == FlightStatus::Departed) {
        if (ac->getType() == AircraftType::PassengerPlane) airportFee = totalTickets * 0.10;
        else if (ac->getType() == AircraftType::CargoPlane) airportFee = ac->getCapacityUnits() * 15.0;
        else airportFee = 5000.0;
    }

    double net = totalTickets - airportFee;
    if (net < 0.0) net = 0.0;

    std::ostringstream out;
    out << "=== Flight Revenue Report: " << flightId << " ===\n";
    out << "Total Ticket Revenue: " << std::fixed << std::setprecision(2) << totalTickets << " EUR\n";
    out << "Airport Tax Deducted: " << std::fixed << std::setprecision(2) << airportFee << " EUR\n";
    out << "Net Profit for Airline: " << std::fixed << std::setprecision(2) << net << " EUR";
    return out.str();
}

std::string AeroPortSystem::airportReport() const {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can view airport report.";
    double passengerTaxes = 0.0;
    double cargoTaxes = 0.0;
    double privateTaxes = 0.0;

    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        if (f.getStatus() != FlightStatus::Departed) continue;
        auto acIt = aircraft.find(f.getAircraftId());
        if (acIt == aircraft.end()) continue;
        const Aircraft* ac = acIt->second.get();

        double totalTickets = 0.0;
        for (int ti : f.getTicketIndexes()) {
            if (ti >=0 && ti < static_cast<int>(tickets.size())) {
                const Ticket& t = tickets[ti];
                if (t.isActive()) totalTickets += t.getPaidAmount();
            }
        }

        if (ac->getType() == AircraftType::PassengerPlane) passengerTaxes += totalTickets * 0.10;
        else if (ac->getType() == AircraftType::CargoPlane) cargoTaxes += ac->getCapacityUnits() * 15.0;
        else if (ac->getType() == AircraftType::PrivateJet) privateTaxes += 5000.0;
    }

    double total = passengerTaxes + cargoTaxes + privateTaxes;
    std::ostringstream out;
    out << "=== Global Airport Financial Report ===\n";
    out << "Taxes from Passenger Flights: " << std::fixed << std::setprecision(2) << passengerTaxes << " EUR\n";
    out << "Taxes from Cargo Flights: " << std::fixed << std::setprecision(2) << cargoTaxes << " EUR\n";
    out << "Taxes from Private Flights: " << std::fixed << std::setprecision(2) << privateTaxes << " EUR\n";
    out << "---------------------------------------\n";
    out << "Total Airport Revenue Generated: " << std::fixed << std::setprecision(2) << total << " EUR";
    return out.str();
}

std::string AeroPortSystem::auditAirline(const std::string& airlineName) const {
    if (!currentUserHasRole(Role::Administrator)) return "[Error] Only the administrator can audit airlines.";
    auto it = airlines.find(airlineName);
    if (it == airlines.end()) return "[Error] Airline '" + airlineName + "' does not exist.";
    const Airline& a = *it->second;

    int completed = 0;
    int cancelled = 0;
    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        if (f.getAirlineName() != airlineName) continue;
        if (f.getStatus() == FlightStatus::Departed) ++completed;
        if (f.getStatus() == FlightStatus::Cancelled) ++cancelled;
    }

    std::ostringstream out;
    out << "=== Airline Audit Report: " << airlineName << " ===\n";
    out << "Total Owned Aircraft: " << a.getAircraftIds().size() << "\n";
    out << "Completed Flights (Departed): " << completed << "\n";
    out << "Cancelled Flights: " << cancelled << "\n";
    out << "Current Corporate Balance: " << std::fixed << std::setprecision(2) << a.getBalance() << " EUR";
    return out.str();
}

std::string AeroPortSystem::listFleet(const std::string& airlineName) const {
    if (!currentUserHasRole(Role::Administrator)) {
        return "[Error] Only the administrator can list airline fleets.";
    }

    const auto airlineIt = airlines.find(airlineName);
    if (airlineIt == airlines.end()) {
        return "[Error] Airline '" + airlineName + "' does not exist.";
    }

    std::ostringstream out;
    out << "Fleet Overview for " << airlineName << ":";
    const auto& aircraftIds = airlineIt->second->getAircraftIds();
    if (aircraftIds.empty()) {
        out << "\n[System] No aircraft owned yet.";
        return out.str();
    }

    for (std::size_t i = 0; i < aircraftIds.size(); ++i) {
        const auto aircraftIt = aircraft.find(aircraftIds[i]);
        if (aircraftIt == aircraft.end()) {
            continue;
        }

        out << "\n" << (i + 1) << ". " << toString(aircraftIt->second->getType())
            << " (ID: " << aircraftIt->second->getId() << ", Model: "
            << aircraftIt->second->getModel() << ") - Health: "
            << aircraftIt->second->getHealth() << "%";
    }

    return out.str();
}

bool AeroPortSystem::userExists(const std::string& username) const {
    return users.find(username) != users.end();
}

bool AeroPortSystem::currentUserHasRole(Role role) const {
    const auto user = currentUser();
    return user && user->getRole() == role;
}

std::shared_ptr<Airline> AeroPortSystem::findAirlineByAircraftId(int aircraftId) {
    const auto aircraftIt = aircraft.find(aircraftId);
    if (aircraftIt == aircraft.end()) {
        return nullptr;
    }

    const auto airlineIt = airlines.find(aircraftIt->second->getAirlineName());
    if (airlineIt == airlines.end()) {
        return nullptr;
    }

    return airlineIt->second;
}

std::shared_ptr<const Airline> AeroPortSystem::findAirlineByAircraftId(int aircraftId) const {
    const auto aircraftIt = aircraft.find(aircraftId);
    if (aircraftIt == aircraft.end()) {
        return nullptr;
    }

    const auto airlineIt = airlines.find(aircraftIt->second->getAirlineName());
    if (airlineIt == airlines.end()) {
        return nullptr;
    }

    return airlineIt->second;
}

std::string AeroPortSystem::listAirspace() const {
    std::string out = "Pending Airspace Traffic (Scheduled / Delayed):";
    int idx = 1;
    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        if (f.getStatus() == FlightStatus::Scheduled || f.getStatus() == FlightStatus::Delayed) {
            const auto aIt = aircraft.find(f.getAircraftId());
            std::string type = (aIt != aircraft.end()) ? toString(aIt->second->getType()) : "Unknown";
            out += "\n" + std::to_string(idx++) + ". " + f.getId() + " [" + type + "] - " + f.getDestination();
        }
    }
    if (idx == 1) out += "\n[System] No pending flights.";
    return out;
}

std::string AeroPortSystem::listRunways() const {
    std::string out = "Airport Runways Overview:";
    for (const auto& kv : runways) {
        out += "\n- " + kv.second.info();
    }
    if (runways.empty()) out += "\n[System] No runways built.";
    return out;
}

static bool validateRunwayForAircraft(const Aircraft& ac, const Runway& rw) {
    switch (ac.getType()) {
        case AircraftType::PassengerPlane:
            return rw.getLength() >= 2000;
        case AircraftType::CargoPlane:
            return rw.getLength() >= 3000 && rw.supportsHeavyDuty();
        case AircraftType::PrivateJet:
            return rw.getLength() >= 1000 && rw.supportsVipTerminal();
    }
    return false;
}

std::string AeroPortSystem::assignRunway(const std::string& flightId, const std::string& runwayId) {
    if (!currentUserHasRole(Role::Dispatcher)) return "[Error] Only a dispatcher can assign runways.";
    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight '" + flightId + "' does not exist.";
    auto rit = runways.find(runwayId);
    if (rit == runways.end()) return "[Error] Runway '" + runwayId + "' does not exist.";

    Runway& rw = rit->second;
    if (!rw.isFree()) return "[Error] Runway " + runwayId + " is currently Occupied or not available!";

    Flight& f = fit->second;
    if (!(f.getStatus() == FlightStatus::Scheduled || f.getStatus() == FlightStatus::Delayed)) {
        return "[Error] Only Scheduled or Delayed flights can be assigned to a runway.";
    }
    const auto aIt = aircraft.find(f.getAircraftId());
    if (aIt == aircraft.end()) return "[Error] Aircraft for flight not found.";
    Aircraft* ac = aIt->second.get();
    if (!validateRunwayForAircraft(*ac, rw)) {
        return std::string("[Error] Runway ") + runwayId + " does not meet aircraft requirements!";
    }

    rw.occupy(ac->getId());
    f.assignRunway(runwayId);

    DispatcherAction act;
    act.type = DispatcherAction::Type::Assign;
    act.flightId = flightId;
    act.runwayId = runwayId;
    actionHistory.push_back(act);

    return "[Success] Flight " + flightId + " assigned to " + runwayId + ". Status -> Boarding. Runway " + runwayId + " -> Occupied.";
}

std::string AeroPortSystem::delayFlight(const std::string& flightId, const std::string& reason) {
    if (!currentUserHasRole(Role::Dispatcher)) return "[Error] Only a dispatcher can delay flights.";
    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight does not exist.";
    Flight& f = fit->second;
    f.setStatus(FlightStatus::Delayed);
    return "[System] Flight " + flightId + " status changed to Delayed. Notification dispatched to passengers.";
}

std::string AeroPortSystem::freeRunway(const std::string& runwayId) {
    if (!currentUserHasRole(Role::Dispatcher)) return "[Error] Only a dispatcher can free runways.";
    auto rit = runways.find(runwayId);
    if (rit == runways.end()) return "[Error] Runway does not exist.";
    Runway& rw = rit->second;
    if (rw.isFree()) return "[Error] Runway is already free.";
    if (!rw.getOccupiedAircraftId().has_value()) return "[Error] Runway has no occupied aircraft.";

    int acId = rw.getOccupiedAircraftId().value();

    Flight* flightPtr = nullptr;
    for (auto& kv : flights) {
        const auto assigned = kv.second.getAssignedRunwayId();
        if (assigned.has_value() && assigned.value() == runwayId) {
            flightPtr = &kv.second;
            break;
        }
    }
    if (!flightPtr) return "[Error] No flight associated with this runway.";
    Flight& f = *flightPtr;

    double totalTickets = 0.0;
    for (int ti : f.getTicketIndexes()) {
        if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
            const Ticket& t = tickets[ti];
            if (t.isActive()) totalTickets += t.getPaidAmount();
        }
    }

    auto aIt = aircraft.find(f.getAircraftId());
    if (aIt == aircraft.end()) return "[Error] Aircraft not found.";
    Aircraft* ac = aIt->second.get();

    double airportFee = 0.0;
    double transferToAirline = 0.0;
    switch (ac->getType()) {
        case AircraftType::PassengerPlane:
            airportFee = totalTickets * 0.10; 
            break;
        case AircraftType::CargoPlane:
            airportFee = 15.0 * static_cast<double>(ac->getCapacityUnits());
            break;
        case AircraftType::PrivateJet:
            airportFee = 5000.0;
            break;
    }

    transferToAirline = totalTickets - airportFee;

    DispatcherAction act;
    act.type = DispatcherAction::Type::Free;
    act.flightId = f.getId();
    act.runwayId = runwayId;
    act.ticketRevenue = totalTickets;
    act.airportFee = airportFee;
    act.transferredToAirline = transferToAirline;
    act.aircraftId = ac->getId();
    act.prevAircraftHealth = ac->getHealth();

    auto airline = findAirlineByAircraftId(ac->getId());
    if (!airline) return "[Error] Airline not found.";
    act.prevAirlineBalance = airline->getBalance();
    act.prevAirportBalance = airportBalance;

    airportBalance += airportFee;
    airline->deposit(transferToAirline);

    rw.free();
    f.setStatus(FlightStatus::Departed);
    f.clearRunway();

    ac->decreaseHealthAfterFlight();

    actionHistory.push_back(act);

    std::ostringstream out;
    out << "[System] Runway " << runwayId << " freed. Flight " << f.getId() << " status -> Departed.\n";
    out << "[Finance] Ticket Sales: " << std::fixed << std::setprecision(2) << totalTickets << " EUR. ";
    if (ac->getType() == AircraftType::CargoPlane) {
        out << "Cargo Tax Deducted (" << ac->getCapacityUnits() << "t * 15 EUR): " << airportFee << " EUR. ";
    } else if (ac->getType() == AircraftType::PassengerPlane) {
        out << "Airport Tax Deducted (10%): " << airportFee << " EUR. ";
    } else {
        out << "Airport Fee Deducted: " << airportFee << " EUR. ";
    }
    out << "Profit for " << findAirlineByAircraftId(ac->getId())->getName() << ": " << std::fixed << std::setprecision(2) << transferToAirline << " EUR.";

    out << "\n[Health] Aircraft ID: " << ac->getId() << " health decreased by " << act.prevAircraftHealth - ac->getHealth() << "% . Current Health: " << ac->getHealth() << "%.";

    return out.str();
}

std::string AeroPortSystem::undo() {
    if (!(currentUserHasRole(Role::Dispatcher) || currentUserHasRole(Role::Administrator))) {
        return "[Error] Only a dispatcher or administrator can undo actions.";
    }
    if (actionHistory.empty()) return "[Error] No dispatcher actions to undo.";

    DispatcherAction act = actionHistory.back();
    actionHistory.pop_back();

    if (act.type == DispatcherAction::Type::Assign) {
        auto rf = runways.find(act.runwayId);
        if (rf != runways.end()) {
            rf->second.free();
        }

        auto ff = flights.find(act.flightId);
        if (ff != flights.end()) {
            ff->second.setStatus(FlightStatus::Scheduled);
            ff->second.clearRunway();
        }

        std::ostringstream out;
        out << "[System] Reverting last command (assign-runway " << act.flightId << "...)\n";
        out << "[System] Flight " << act.flightId << " status reverted to Scheduled. Runway " << act.runwayId << " reverted to Free.";
        return out.str();
    }

    if (act.type == DispatcherAction::Type::HangarAdmit) {
        auto hIt = hangars.find(act.runwayId);
        if (hIt != hangars.end()) {
            hIt->second.releaseAircraft(act.aircraftId);
        }
        auto airline = findAirlineByAircraftId(act.aircraftId);
        if (airline) {
            airline->withdraw(airline->getBalance());
            airline->deposit(act.prevAirlineBalance);
        }

        std::ostringstream out;
        out << "[System] Reverting last command (send-to-hangar " << act.aircraftId << " to " << act.runwayId << "...)\n";
        out << "[System] Aircraft " << act.aircraftId << " removed from Hangar " << act.runwayId << ". Finances restored.";
        return out.str();
    }

    if (act.type == DispatcherAction::Type::HangarRetrieve) {
        auto hIt = hangars.find(act.runwayId);
        if (hIt != hangars.end()) {
            hIt->second.admitAircraft(act.aircraftId);
        }
        auto acIt = aircraft.find(act.aircraftId);
        if (acIt != aircraft.end()) {
            acIt->second->setHealth(act.prevAircraftHealth);
        }

        std::ostringstream out;
        out << "[System] Reverting last command (retrieve-from-hangar " << act.aircraftId << "...)\n";
        out << "[System] Aircraft " << act.aircraftId << " re-admitted to Hangar " << act.runwayId << ". Health restored to " << act.prevAircraftHealth << "%.";
        return out.str();
    }

    auto rf = runways.find(act.runwayId);
    auto ff = flights.find(act.flightId);
    auto acIt = aircraft.find(act.aircraftId);
    std::shared_ptr<Airline> airline;
    if (acIt != aircraft.end()) airline = findAirlineByAircraftId(acIt->second->getId());

    if (rf != runways.end()) {
        rf->second.occupy(act.aircraftId);
    }
    if (ff != flights.end()) {
        ff->second.assignRunway(act.runwayId);
        ff->second.setStatus(FlightStatus::Boarding);
    }
    if (acIt != aircraft.end()) {
        acIt->second->setHealth(act.prevAircraftHealth);
    }
    if (airline) {
        airline->withdraw(airline->getBalance());
        airline->deposit(act.prevAirlineBalance);
    }
    airportBalance = act.prevAirportBalance;

    std::ostringstream out;
    out << "[System] Reverting last command (free-runway " << act.runwayId << "...)\n";
    out << "[System] Flight " << act.flightId << " status reverted to Boarding. Runway " << act.runwayId << " reverted to Occupied. Finances and Health states restored!";
    return out.str();
}
std::string AeroPortSystem::addFunds(double amount) {
    if (!currentUserHasRole(Role::Passenger)) {
        return "[Error] Only a passenger can add funds.";
    }
    if (amount <= 0.0) {
        return "[Error] Amount must be positive.";
    }
    auto p = currentUser();
    if (!p) {
        return "[Error] Internal user type mismatch.";
    }
    p->passengerAddFunds(amount);
    std::ostringstream out;
    out << "[System] Funds added successfully. New balance: " << std::fixed << std::setprecision(2)
        << p->getPassengerBalance() << " EUR.";
    return out.str();
}

std::string AeroPortSystem::listFlights(const std::string& destination) const {
    std::ostringstream out;
    out << "Available flights to " << destination << ":";
    int idx = 1;
    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        if (f.getDestination() == destination) {
            if (f.getStatus() == FlightStatus::Scheduled || f.getStatus() == FlightStatus::Delayed) {
                out << "\n" << idx++ << ". " << f.getId() << " | Status: " << toString(f.getStatus())
                    << " | Base Price: " << std::fixed << std::setprecision(2) << f.getBasePrice() << " EUR";
            }
        }
    }
    if (idx == 1) {
        out << "\n[System] No flights found.";
    }
    return out.str();
}

std::string AeroPortSystem::filterFlights(double maxPrice) const {
    std::ostringstream out;
    out << "Filtered flights (Price < " << std::fixed << std::setprecision(2) << maxPrice << " EUR):";
    int idx = 1;
    for (const auto& kv : flights) {
        const Flight& f = kv.second;
        if (f.getBasePrice() < maxPrice) {
            out << "\n" << idx++ << ". " << f.getId() << " (" << f.getDestination() << ") - "
                << std::fixed << std::setprecision(2) << f.getBasePrice() << " EUR";
        }
    }
    if (idx == 1) {
        out << "\n[System] No flights match the filter.";
    }
    return out.str();
}

static double computeTicketPrice(double basePrice, TicketType type) {
    switch (type) {
        case TicketType::Standard: return basePrice;
        case TicketType::LastMinute: return basePrice * 0.5;
        case TicketType::VIP: return basePrice * 2.0;
    }
    return basePrice;
}

std::string AeroPortSystem::bookTicket(const std::string& flightId, TicketType type) {
    if (!currentUserHasRole(Role::Passenger)) {
        return "[Error] Only a passenger can book tickets.";
    }
    auto fit = flights.find(flightId);
    if (fit == flights.end()) {
        return "[Error] Flight '" + flightId + "' does not exist.";
    }
    Flight& flight = fit->second;
    if (!(flight.getStatus() == FlightStatus::Scheduled || flight.getStatus() == FlightStatus::Delayed)) {
        return "[Error] Tickets can only be purchased for Scheduled or Delayed flights.";
    }

    auto aIt = aircraft.find(flight.getAircraftId());
    if (aIt == aircraft.end()) {
        return "[Error] Aircraft for flight not found.";
    }
    Aircraft* ac = aIt->second.get();
    if (type == TicketType::VIP && ac->getType() == AircraftType::CargoPlane) {
        return "[Error] You cannot purchase VIP tickets for Cargo flights!";
    }

    int capacityUnits = ac->getCapacityUnits();
    int occupied = 0;
    for (int ti : flight.getTicketIndexes()) {
        if (ti >= 0 && ti < static_cast<int>(tickets.size()) && tickets[ti].isActive()) {
            ++occupied;
        }
    }
    if (occupied >= capacityUnits) {
        return "[Error] No free seats available on this flight.";
    }

    auto p = currentUser();
    if (!p) return "[Error] Internal user type mismatch.";

    double price = computeTicketPrice(flight.getBasePrice(), type);
    double baggageKg = (type == TicketType::VIP) ? 20.0 : 0.0;
    if (!p->passengerWithdraw(price)) {
        std::ostringstream o; o << "[Error] Insufficient funds! Your balance: " << std::fixed << std::setprecision(2) << p->getPassengerBalance() << " EUR. Ticket price: " << price << " EUR.";
        return o.str();
    }

    tickets.emplace_back(p->getUsername(), flightId, type, price, baggageKg);
    int ticketIndex = static_cast<int>(tickets.size()) - 1;
    flight.addTicketIndex(ticketIndex);

    std::ostringstream out;
    out << "[Success] " << toString(type) << " ticket booked for " << flightId << ". Paid: " << std::fixed << std::setprecision(2) << price << " EUR. Current balance: " << p->getPassengerBalance() << " EUR.";
    return out.str();
}

std::string AeroPortSystem::upgradeTicket(const std::string& flightId, TicketType newType) {
    if (!currentUserHasRole(Role::Passenger)) return "[Error] Only a passenger can upgrade tickets.";
    auto p = currentUser();
    if (!p) return "[Error] Internal user type mismatch.";

    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight does not exist.";
    Flight& flight = fit->second;

    int foundIdx = -1;
    for (int ti : flight.getTicketIndexes()) {
        if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
            const Ticket& t = tickets[ti];
            if (t.isActive() && t.getOwnerName() == p->getUsername()) {
                foundIdx = ti;
                break;
            }
        }
    }
    if (foundIdx == -1) return "[Error] No active ticket for this flight found.";

    Ticket& ticket = tickets[foundIdx];
    if (ticket.getType() == newType) return "[Error] Ticket is already of the requested type.";

    double basePrice = flight.getBasePrice();
    double currentBasePrice = computeTicketPrice(basePrice, ticket.getType());
    double newBasePrice = computeTicketPrice(basePrice, newType);
    double diff = newBasePrice - currentBasePrice;
    if (diff <= 0.0) return "[Error] Upgrade must be to a more expensive ticket type.";
    if (!p->passengerWithdraw(diff)) {
        return std::string("[Error] Insufficient funds! Need: ") + formatMoney(diff) + " EUR.";
    }

    double includedBaggage = (newType == TicketType::VIP) ? 20.0 : 0.0;
    ticket.upgrade(newType, diff, includedBaggage);

    std::string out = "[Success] Ticket upgraded to " + toString(newType) + ". Paid difference: " + formatMoney(diff) + " EUR. Current balance: " + formatMoney(p->getPassengerBalance()) + " EUR.";
    return out;
}

std::string AeroPortSystem::addBaggage(const std::string& flightId, double weightKg) {
    if (!currentUserHasRole(Role::Passenger)) return "[Error] Only a passenger can add baggage.";
    if (weightKg <= 0.0) return "[Error] Baggage weight must be positive.";
    auto p = currentUser();
    if (!p) return "[Error] Internal user type mismatch.";

    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight does not exist.";
    Flight& flight = fit->second;

    int foundIdx = -1;
    for (int ti : flight.getTicketIndexes()) {
        if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
            Ticket& t = tickets[ti];
            if (t.isActive() && t.getOwnerName() == p->getUsername()) {
                foundIdx = ti;
                break;
            }
        }
    }
    if (foundIdx == -1) return "[Error] No active ticket for this flight found.";

    double fee = 5.0 * weightKg;
    if (!p->passengerWithdraw(fee)) {
        return std::string("[Error] Insufficient funds! Baggage fee: ") + formatMoney(fee) + " EUR.";
    }

    tickets[foundIdx].addBaggage(weightKg, fee);
    std::string out = "[Success] Added " + formatNumber(weightKg) + " kg baggage to " + flightId + " ticket. Fee: " + formatMoney(fee) + " EUR. Current balance: " + formatMoney(p->getPassengerBalance()) + " EUR.";
    return out;
}

std::string AeroPortSystem::cancelTicket(const std::string& flightId) {
    if (!currentUserHasRole(Role::Passenger)) return "[Error] Only a passenger can cancel tickets.";
    auto p = currentUser();
    if (!p) return "[Error] Internal user type mismatch.";

    auto fit = flights.find(flightId);
    if (fit == flights.end()) return "[Error] Flight does not exist.";
    Flight& flight = fit->second;

    int chosen = -1;
    int bestRank = -1;
    for (int ti : flight.getTicketIndexes()) {
        if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
            Ticket& t = tickets[ti];
            if (!t.isActive()) continue;
            if (t.getOwnerName() != p->getUsername()) continue;
            if (!t.isRefundable()) continue;
            int rank = (t.getType() == TicketType::VIP) ? 3 : 2;
            if (rank > bestRank) { bestRank = rank; chosen = ti; }
        }
    }
    if (chosen == -1) return "[Error] No refundable ticket found for this flight.";

    double refund = tickets[chosen].getPaidAmount();
    tickets[chosen].cancel();
    p->passengerDeposit(refund);

    std::string out = "[Success] " + toString(tickets[chosen].getType()) + " ticket for " + flightId + " cancelled. Refunded: " + formatMoney(refund) + " EUR. Current balance: " + formatMoney(p->getPassengerBalance()) + " EUR.";
    return out;
}

std::string AeroPortSystem::myTickets() const {
    if (!currentUserHasRole(Role::Passenger)) return "[Error] Only a passenger can view tickets.";
    auto p = currentUser();
    if (!p) return "[Error] Internal user type mismatch.";

    std::string out = "Your active tickets:";
    int idx = 1;
    for (const auto& fkv : flights) {
        const Flight& f = fkv.second;
        for (int ti : f.getTicketIndexes()) {
            if (ti >= 0 && ti < static_cast<int>(tickets.size())) {
                const Ticket& t = tickets[ti];
                if (!t.isActive()) continue;
                if (t.getOwnerName() != p->getUsername()) continue;
                out += "\n" + std::to_string(idx++) + ". Flight: " + f.getId() + " | Type: " + toString(t.getType())
                    + " | Paid: " + formatMoney(t.getPaidAmount())
                    + " EUR | Baggage: " + formatNumber(t.getBaggageKg()) + " kg | Status: " + toString(f.getStatus());
            }
        }
    }
    if (idx == 1) out += "\n[System] No active tickets.";
    return out;
}
