#include "Apportionment.h"
#include "HeapPQueue.h"
using namespace std;

Map<string, int> apportion(const Map<string, int>& populations, int numSeats) {
    // Errors
    // There is no state
    if (populations.isEmpty()) {
        error("No state!");
        return {};
    }
    // The number of seats is smaller than the number of states
    if (numSeats < populations.size()) {
        error("Too few seats!");
        return {};
    }
    Map<string, int> result;
    // Any state has 0 or negative population
    for (string state : populations) {
        int pop = populations[state];
        if (pop <= 0) {
            error("Invalid population!");
            return {};
        }
        // Initially give every state a seat
        result[state] = 1;
    }

    // Initially give every state a seat
    int seat = numSeats - populations.size();

    HeapPQueue pq;
    // Enqueue the information of state - Initialize
    // HeapPQueue gives out the lowest weight result by default, so we reverse the population weight by 1/population
    for (string state : populations) {
        double pop = static_cast<double> (populations[state]);
        DataPoint tmp;
        tmp.name = state;
        tmp.weight = sqrt(2)/pop;
        pq.enqueue(tmp);
    }

    while (seat > 0) {
        DataPoint currAdd = pq.dequeue();
        string state = currAdd.name;
        result[currAdd.name] += 1;
        seat--;

        double pop = static_cast<double> (populations[state]);
        currAdd.weight = sqrt(result[state] * (result[state] + 1)) / pop;
        pq.enqueue(currAdd);
    }

    return result;
}


/* * * * * Test Cases Below This Point * * * * */
#include "GUI/SimpleTest.h"
#include "Utilities/CSV.h"

PROVIDED_TEST("Each state gets at least one seat.") {
    /* Fall 2020 */
    Map<string, int> populations = {
        { "CS106A", 551 },
        { "CS106B", 428 },
        { "CS103",  206 },
        { "CS107",  267 },
        { "CS109",  214 },
        { "CS110",  196 },
        { "CS161",  187 }
    };

    /* Seven seats and seven classes means everyone gets one. */
    Map<string, int> seats = {
        { "CS106A", 1 },
        { "CS106B", 1 },
        { "CS103",  1 },
        { "CS107",  1 },
        { "CS109",  1 },
        { "CS110",  1 },
        { "CS161",  1 }
    };

    EXPECT_EQUAL(apportion(populations, 7), seats);
}

PROVIDED_TEST("Reports an error if there are more states than seats.") {
    Map<string, int> populations = {
        { "House",         435 },
        { "Senate",        100 },
        { "Supreme Court",   9 },
        { "President",       1 },
    };

    EXPECT_ERROR(apportion(populations, 3));
    EXPECT_ERROR(apportion(populations, -2));
    EXPECT_NO_ERROR(apportion(populations, 4));
}

PROVIDED_TEST("Reports an error if there are no states.") {
    EXPECT_ERROR(apportion({ }, 3));
}

PROVIDED_TEST("Works on the example from the handout.") {
    Map<string, int> populations = {
        { "Mind",    25419 },
        { "Denial",  13509 },
        { "Affairs", 7984  },
        { "Being",   14938 },
        { "Matter",  14617 }
    };

    Map<string, int> seats = {
        { "Mind",    5 },
        { "Denial",  2 },
        { "Affairs", 1 },
        { "Being",   3 },
        { "Matter",  3 }
    };

    EXPECT_EQUAL(apportion(populations, 14), seats);
}

namespace {
    Map<string, int> populationsFrom(const string& filename) {
        CSV csv = CSV::parseFile(filename);

        Map<string, int> populations;
        for (size_t i = 0; i < csv.numRows(); i++) {
            populations[csv[i]["State"]] = stringToInteger(csv[i]["Population"]);
        }

        return populations;
    }
}

PROVIDED_TEST("Works on the 2020 apportionment.") {
    auto populations = populationsFrom("res/apportionment/2020.csv");

    /* Apportioned results. */
    Map<string, int> seats = {
        { "Alabama", 7 },
        { "Alaska", 1 },
        { "Arizona", 9 },
        { "Arkansas", 4 },
        { "California", 52 },
        { "Colorado", 8 },
        { "Connecticut", 5 },
        { "Delaware", 1 },
        { "Florida", 28 },
        { "Georgia", 14 },
        { "Hawaii", 2 },
        { "Idaho", 2 },
        { "Illinois", 17 },
        { "Indiana", 9 },
        { "Iowa", 4 },
        { "Kansas", 4 },
        { "Kentucky", 6 },
        { "Louisiana", 6 },
        { "Maine", 2 },
        { "Maryland", 8 },
        { "Massachusetts", 9 },
        { "Michigan", 13 },
        { "Minnesota", 8 },
        { "Mississippi", 4 },
        { "Missouri", 8 },
        { "Montana", 2 },
        { "Nebraska", 3 },
        { "Nevada", 4 },
        { "New Hampshire", 2 },
        { "New Jersey", 12 },
        { "New Mexico", 3 },
        { "New York", 26 },
        { "North Carolina", 14 },
        { "North Dakota", 1 },
        { "Ohio", 15 },
        { "Oklahoma", 5 },
        { "Oregon", 6 },
        { "Pennsylvania", 17 },
        { "Rhode Island", 2 },
        { "South Carolina", 7 },
        { "South Dakota", 1 },
        { "Tennessee", 9 },
        { "Texas", 38 },
        { "Utah", 4 },
        { "Vermont", 1 },
        { "Virginia", 11 },
        { "Washington", 10 },
        { "West Virginia", 2 },
        { "Wisconsin", 8 },
        { "Wyoming", 1 },
    };

    EXPECT_EQUAL(apportion(populations, 435), seats);
}

PROVIDED_TEST("Stress test: works on a large number of states.") {
    /* 5s is a 10x margin of safety above our reference solution. */
    EXPECT_COMPLETES_IN(5.0,
        const int kLargeNumber = 100000;

        /* kLargeNumber of states with population 1. */
        Map<string, int> populations;
        for (int i = 0; i < kLargeNumber; i++) {
            populations[to_string(i)] = 1;
        }

        /* One state with population kLargeNumber. */
        populations[to_string(kLargeNumber)] = kLargeNumber;

        /* Apportion kLargeNumber + (kLargeNumber / 4 - 1) seats. This has the effect
         * of giving everyone one seat, then giving kLargeNumber the remaining
         * kLargeNumber / 4 - 1 seats.
         */
        const int kExtraSeats = kLargeNumber / 4 - 1;

        auto result = apportion(populations, kLargeNumber + kExtraSeats);

        /* Confirm all the small states get one seat. */
        for (int i = 0; i < kLargeNumber; i++) {
            EXPECT_EQUAL(result[to_string(i)], 1);
        }

        /* Confirm the big state gets the rest. */
        EXPECT_EQUAL(result[to_string(kLargeNumber)], kExtraSeats);
    );
}
