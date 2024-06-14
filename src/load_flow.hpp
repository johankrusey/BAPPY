#ifndef LOAD_FLOW_GAUSS_SEIDEL
#define LOAD_FLOW_GAUSS_SEIDEL

#include <vector>
#include <complex>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#define width 10
#define YbusWidth 22
#define PI 3.14159265358979323846

using namespace std;

class load_flow // This is code largely based on the code found at: https://github.com/Gopal-Dahale/Load-flow-analysis
    // We added multiple functions and even edited the code at some places, but most of the code remains intact and most of the comments are his, I will preface my comments with JK-
{
private:
    int read(int index);
    void init_S(); // initialising S // busdata, S, N
    void init_V(); // initialising V // busData, V, N
    auto guass_seidel(int, int, bool);
    void update();
    void output();
    vector<string> calculateLineLoading();
    void outputLineData();
    auto reactive_power(int, int);
    auto real_power(int, int);
    void assignSlackLoad(); // Function to assign slack load based on weights

public:
    load_flow(string, string, int, int);  // Constructor to initialize;
    int solar_bus_number = 47;
    int wind_bus_number = 46;
    int total_busses = 47;
    int iterations, N;                     // JK - The amount of iterations that you want the Gauss-Seidel to run
    string header, busInputData, lineInputData; // size of busData
    vector<vector<double>> busData, lineData;   // 0 base indexing
    vector<complex<double>> S;                  // complex power , 1 base indexing
    vector<vector<complex<double>>> V;          // voltages , 1 base indexing
    vector<vector<complex<double>>> Y;
    vector<bool> canBeSlack;                    // To indicate if a bus can be a slack node
    vector<double> slackWeights;                // To indicate the weight of slack each bus can handle
    void calculateYBus();
    void printBusData();
    void printLineData();
    void printYbus();
    void solveLoadFlow();
    void dothepowerthing(int scenario, int index, float solar, float wind);
};

/*
bus input data format:                                     JK - Make sure that your .csv files follow this format
    type of bus, V, δ, Pg, Qg, Pl ,Ql ,Qg_max, Qg_min, CanBeSlack, SlackWeight
    type of bus is an integer 1-3
    1: slack (there can be only one slack bus)
    2: PV / Gen bus
    3: PQ / Load bus
line input data format:
    from bus, to bus, R, X,G,B, Max MVA
Assumptions:
    * Bus1 is slack bus
    * G=0;
*/


// Define the global variable to store the last working iteration
int last_working_iteration = 0;

// Constructor definition
load_flow::load_flow(string busInputData, string lineInputData, int iter, int index) // JK - This is the main function that calls all of the subfunctions defined in this document
{
    this->busData.clear();
    this->iterations = iter;
    this->busInputData = busInputData;
    this->lineInputData = lineInputData;

    // Update the last working iteration with the return value of read(index)
    last_working_iteration = read(index);

    this->N = this->busData.size();
    this->init_S();
    this->init_V();
    return;
}

int load_flow::read(int index) // JK - Reads the two input data files
{
    string line, word;
    vector<double> row;

    fstream fin;
    fin.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\busInputData.csv", ios::in);//"C:\\Users\\joran\\OneDrive\\Bureaublad\\Godot_v4.2.2-stable_mono_win64\\gdextension_cpp_example\\src\\busInputData.csv", ios::in); // JK - Hardcoded file for busInputData

    if (!fin.is_open())
    {
        cout << "Bus input data file does not exist. Remember to add file name with extension\nDid you change the 5 hardcoded paths in this project?\n";
        return last_working_iteration; // Use return instead of exit to allow for better error handling
    }

    getline(fin, line, '\n'); // Read header
    this->header = line;
    cout << "Starting with time index: " << index << ".\n";
    int throwawaybus = total_busses * index;
    for (int k = 0; k < throwawaybus; ++k)
    {
        getline(fin, line, '\n'); // Skip lines to reach the desired starting point
    }

    int j = 0;
    while (getline(fin, line, '\n'))
    {
        stringstream s(line);
        row.clear();
        // Read each value, handling empty fields
        for (int col = 0; col < 11; ++col)
        {
            if (!getline(s, word, ','))
            {
                word = "0"; // Default to 0 if the value is missing
            }

            try
            {
                double value = stod(word);
                row.push_back(value);
            }
            catch (const invalid_argument&)
            {
                row.push_back(0); // If conversion fails, default to 0
            }
        }



        // Append row to busData
        this->busData.push_back(row);

        j++;
        if (j == total_busses)
        {
            last_working_iteration = index; // Update the last working index after successful read
            cout << "Updated last_working_iteration to: " << last_working_iteration << ".\n";
            break;
        }
    }
    if (fin.eof()){
        // Check if all values in the row are zero
        cout << "Timestep " << index << " is past the final timestep.\nRerunning with the final timestep, timestep " << last_working_iteration << ".\n";
        fin.close();
        this->busData.clear(); // Clear any partially filled data
        return read(last_working_iteration); // Recursively call the function with the last working index
    }
    fin.close();

    fin.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\lineInputData.csv", ios::in); //"C:\\Users\\joran\\OneDrive\\Bureaublad\\Godot_v4.2.2-stable_mono_win64\\gdextension_cpp_example\\src\\lineInputData.csv", ios::in); // JK - Hardcoded file for lineInputData
    if (!fin.is_open())
    {
        cout << "Line input data file does not exist. Remember to add file name with extension\nDid you change the 5 hardcoded paths in this project?\n";
        exit(0);
    }
    else
    {
        getline(fin, line, '\n'); // Read header

        while (getline(fin, line, '\n'))
        {
            stringstream s(line);
            row.clear();

            // Read each value, handling empty fields
            for (int col = 0; col < 7; ++col)
            {
                if (!getline(s, word, ','))
                {
                    word = "0"; // Default to 0 if the value is missing
                }

                try
                {
                    double value = stod(word);
                    row.push_back(value);
                }
                catch (const invalid_argument&)
                {
                    row.push_back(0); // If conversion fails, default to 0
                }
            }

            // Ensure Max MVA is valid
            if (row[6] == 0)
            {
                cout << "Error: Max MVA for line from Bus " << row[0] << " to Bus " << row[1] << " is zero. Skipping this line." << endl;
                continue;
            }

            // Append row to lineData
            this->lineData.push_back(row);
        }

        fin.close();
    }

    // Initialize canBeSlack and slackWeights based on busData
    for (const auto& bus : this->busData)
    {
        this->canBeSlack.push_back(static_cast<bool>(bus[9]));
        this->slackWeights.push_back(bus[10]);
    }

    return last_working_iteration;
}

void load_flow::init_S() // JK - Initializes the power elements that should be guessed via Gauss-Seidel
{
    S = vector<complex<double>>(N + 1, 0);
    for (int i = 0; i < this->N; i++)
    {
        switch ((int)busData[i][0])
        {
        case 2: // PV Bus
            this->S[i + 1].real(this->busData[i][3] - this->busData[i][5]);
            // Ensure busData reflects this initialization
            this->busData[i][3] = this->S[i + 1].real(); // Update Pg
            // cout << "Initializing PV Bus " << i + 1 << " Pg to " << this->S[i + 1].real() << " MW" << endl; // Debug output
            break;
        case 3: // PQ Bus
            this->S[i + 1].real(0); // Set generation real power to 0 for PQ bus
            this->S[i + 1].imag(0); // Set generation reactive power to 0 for PQ bus
            // Ensure busData reflects this initialization
            this->busData[i][3] = 0; // Update Pg
            this->busData[i][4] = 0; // Update Qg
            // cout << "Initializing PQ Bus " << i + 1 << " Pg to " << this->S[i + 1].real() << " MW and Qg to " << this->S[i + 1].imag() << " Mvar" << endl; // Debug output
            break;
        default:
            break;
        }
    }
    return;
}

void load_flow::printLineData() // JK - debugging function, not currently integrated
{
    for (int i = 0; i < this->lineData.size(); i++)
    {
        for (int j = 0; j < this->lineData[i].size(); j++)
            cout << left << setw(width) << setfill(' ') << this->lineData[i][j];
        cout << "\n";
    }
    return;
}

void dothepowerthing(int scenario, int index, float solar, float wind) {
    if (scenario == 0) {
        load_flow obj("busInputData.csv", "lineInputData.csv", 500, index); //JK - For a grid up to about 50 busses 30 to 50 iterations are needed for higher accuracy
        if (obj.solar_bus_number <= obj.N) {
            obj.busData[obj.solar_bus_number - 1][3] = solar; // Update Pg value for solar bus
            obj.S[obj.solar_bus_number].real(solar - obj.busData[obj.solar_bus_number - 1][5]); // Update S for solar bus
/*             cout << "Updated Solar Bus " << obj.solar_bus_number << " Pg to " << solar << " MW" << endl; // Debug output */
        }
        if (obj.wind_bus_number <= obj.N) {
            obj.busData[obj.wind_bus_number - 1][3] = wind; // Update Pg value for wind bus
            obj.S[obj.wind_bus_number].real(wind - obj.busData[obj.wind_bus_number - 1][5]); // Update S for wind bus
/*             cout << "Updated Wind Bus " << obj.wind_bus_number << " Pg to " << wind << " MW" << endl; // Debug output */
        }

        obj.calculateYBus();
        obj.solveLoadFlow();
    }
    return;
}

void load_flow::init_V() // JK - Initializes the voltage elements that should be guessed via gauss seidel
{
    V = vector<vector<complex<double>>>(N + 1);
    this->V[1].push_back(complex<double>(1, 0)); // Sets the slack bus voltage
    this->V[solar_bus_number].push_back(complex<double>(1, 0));
    this->V[wind_bus_number].push_back(complex<double>(1, 0));

    // Ensure the busData for slack bus reflects this initialization
    if (this->busData.size() > 0) {
        this->busData[0][1] = 1; // Voltage magnitude
    }

    for (int i = 1; i < this->N; i++)
    {
        switch ((int)this->busData[i][0])
        {
        case 2: // PV BUS
            this->V[i + 1].push_back(complex<double>(this->busData[i][1], 0));
            this->busData[i][1] = V[i + 1].back().real();
            break;

        case 3: // PQ BUS
            this->V[i + 1].push_back(complex<double>(1, 0));
            // Ensure busData reflects this initialization
            this->busData[i][1] = 1; // Voltage magnitude
            //cout << "Initializing PQ Bus " << i + 1 << " Voltage to 1.0 p.u." << endl; // Debug output
            break;
        default:
            break;
        }
    }
    return;
}

/* prints bus data. data is updated after calling solveLoadFlow() */
void load_flow::printBusData()
{
    for (int i = 0; i < this->N; i++)
    {
        cout << "bus " << i + 1 << "   ";
        for (int j = 1; j < this->busData[i].size(); j++)
            cout << left << setw(width) << setfill(' ') << this->busData[i][j];
        cout << "\n";
    }
    return;
}

/* calculates the Y Bus matrix */
void load_flow::calculateYBus()
{
    // Initialize Ybus matrix with zeros
    Y = vector<vector<complex<double>>>(N + 1, vector<complex<double>>(N + 1, 0));
    complex<double> y1, y2, one(1, 0);
    y2.real(0);

    // Process each line and update the Ybus matrix
    for (int i = 0; i < this->lineData.size(); i++)
    {
        int from = static_cast<int>(this->lineData[i][0]);
        int to = static_cast<int>(this->lineData[i][1]);
        double r = this->lineData[i][2];
        double x = this->lineData[i][3];
        double b = this->lineData[i][5] / 2;

        // Calculate admittance
        y1 = one / complex<double>(r, x);
        y2.imag(b);
        
        // Update off-diagonal elements
        this->Y[from][to] -= y1;
        this->Y[to][from] -= y1;

        // Update diagonal elements
        this->Y[from][from] += (y1 + y2);
        this->Y[to][to] += (y1 + y2);

        // Debugging output
        //cout << "Processed line from " << from << " to " << to << " with admittance " << y1 << " and shunt " << y2 << endl;
    }

    // Print the Ybus matrix for debugging
/*     cout << "Ybus matrix:" << endl;
    for (int i = 1; i <= this->N; i++)
    {
        for (int j = 1; j <= this->N; j++)
        {
            cout << left << setw(YbusWidth) << setfill(' ') << this->Y[i][j];
        }
        cout << "\n";
    } */
    return;
}
    
void load_flow::printYbus() // JK - debugging function, not currently integrated
{
    for (int i = 1; i <= this->N; i++)
    {
        for (int j = 1; j <= this->N; j++)
            cout << left << setw(YbusWidth) << setfill(' ') << this->Y[i][j];
        cout << "\n";
    }
    for (int i = 0; i < Y.size(); ++i) {
        double sum = 0;
        for (int j = 0; j < Y[i].size(); ++j) {
            if (i != j) {
                sum += abs(Y[i][j]);
            }
        }
    }
    return;
}

auto load_flow::guass_seidel(int itr, int k, bool flag) // JK - the actual guass-seidel calculations
{
    complex<double> sum(0, 0);
    for (int n = 1; n <= this->N; n++)
    {
        if (n < k)
            sum += this->Y[k][n] * this->V[n][itr];
        else if (n > k)
            sum += this->Y[k][n] * this->V[n][itr - 1];
    }
    auto denom = this->Y[k][k];
    auto numerator = (flag) ? conj(this->S[k] / this->V[k][itr]) - sum : conj(this->S[k] / this->V[k][itr - 1]) - sum;
    auto result = numerator / denom;
/*     cout << "bus " << k << " iteration " << itr << " PQ conjugate: " << conj(this->S[k]) << endl;
    cout << "bus " << k << " iteration " << itr << " Voltage conjugate: " << this->V[k][itr] << endl;
    cout << "bus " << k << " iteration " << itr << " sum: " << sum << endl;
    cout << "bus " << k << " iteration " << itr << " numerator: " << numerator << endl;
    cout << "bus " << k << " iteration " << itr << " denominator: " << denom << endl; */
    /* cout << "bus " << k << " iteration " << itr << " new voltage: " << result << endl;
    cout << "bus " << k << " iteration " << itr << " old voltage: " << this->V[k][itr-1] << endl;
    cout << endl;   */
    if (abs(abs(result) - abs(this->V[k][itr - 1])) > 0.5){
        cout << "Large voltage difference detected, new: " << result << " old: " << this->V[k][itr - 1] << endl;
        cout << "Values: S[k]= " << this->S[k] << " Y[k][k] (diagonal): " << this->Y[k][k] << endl;
        cout << endl;
    }

    if (isnan(abs(result.real())) || isnan(abs(result.imag()))) {
        cout << "NaN detected in Gauss-Seidel calculation at Bus " << k << ", Iteration " << itr << endl;
        for (int n = 1; n <= this->N; n++)
        {
            if (n < k){
                cout << this->Y[k][n] * this->V[n][itr] << " , " << n << endl;
                cout << "Admittance: " << this->Y[k][n] << " Voltage: " << this->V[n][itr] << endl;
            }
            else if (n > k){
                cout << this->Y[k][n] * this->V[n][itr - 1] << " , " << n << endl;
            }
        }
        cout << "Values: S[k]=" << this->S[k] << ", V[k][itr]=" << this->V[k][itr] << ", sum=" << sum << endl;
        cout << "Numerator: " << numerator << ", Denominator: " << denom << endl;
        cout << "Y[k][k] (diagonal): " << this->Y[k][k] << endl; // Check diagonal element
        cout << "Bus Data: ";
        for (int i = 0; i < this->busData[k - 1].size(); i++)
            cout << this->busData[k - 1][i] << " ";
        cout << endl;
    }

    return result;
}

auto load_flow::reactive_power(int itr, int k) // JK - This function calculates the reactive power in the grid for each bus
{
    double mag{ abs(this->V[k][itr]) };
    double angle{ arg(this->V[k][itr]) };
    double sum{ 0 };
    for (int n = 1; n <= this->N; n++)
        sum += abs(this->V[n][itr]) * (this->Y[k][n].real() * sin(angle - arg(this->V[n][itr])) - this->Y[k][n].imag() * cos(angle - arg(this->V[n][itr])));

    sum *= mag;

    if (isnan(sum)) {
        cout << "NaN detected in reactive power calculation for Bus " << k << ", Iteration " << itr << endl;
    }

    return sum;
}

auto load_flow::real_power(int itr, int k) // JK - This function calculates the real power in the grid for each bus
{
    double mag{ abs(this->V[k][itr]) };
    double angle{ arg(this->V[k][itr]) };
    double sum{ 0 };
    for (int n = 1; n <= this->N; n++) {
        sum += abs(this->V[n][itr]) * (this->Y[k][n].real() * cos(angle - arg(this->V[n][itr])) + this->Y[k][n].imag() * sin(angle - arg(this->V[n][itr])));
    }
    sum *= mag;

    if (isnan(sum)) {
        cout << "NaN detected in real power calculation for Bus " << k << ", Iteration " << itr << endl;
    }

    return sum;
}

void load_flow::update() // Function that updates all the vales while running the gauss-seidel
{
    for (int i = 1; i <= this->N; i++)
    {
        this->busData[i - 1][1] = abs(this->V[i][this->iterations]);            // V
        this->busData[i - 1][2] = arg(this->V[i][this->iterations]) * 180 / PI; // delta in degrees
        this->busData[i - 1][3] = this->S[i].real() + this->busData[i - 1][5];  // Pg
        this->busData[i - 1][4] = this->S[i].imag() + this->busData[i - 1][6];  // Qg
    }
    return;
}

vector<string> load_flow::calculateLineLoading() // JK - Custom function that calculates the load on each of the lines as a percentage of its capacity
{
    vector<string> loadingResults;
    loadingResults.push_back("From Bus,To Bus,Loading (%),Direction"); // JK - Header for line loading CSV

    for (const auto& line : this->lineData)
    {
        int from = line[0];
        int to = line[1];
        double maxMVA = line[6];

        // Ensure Max MVA is valid
        if (maxMVA == 0)
        {
            cout << "Error: Max MVA for line from Bus " << from << " to Bus " << to << " is zero. Skipping this line." << endl;
            continue;
        }

        complex<double> V_from = this->V[from].back();
        complex<double> V_to = this->V[to].back();
        complex<double> Z_line(line[2], line[3]); // JK - R + jX

        complex<double> I_line = (V_from - V_to) / Z_line;
        complex<double> S_line = V_from * conj(I_line);
/*         cout << "from " << from << " to "<< to << " current: " << I_line << endl; */
        double loadingPercent = std::abs(S_line) / maxMVA * 100;
        int direction = (abs(V_from) > abs(V_to)) ? 1 : -1; // JK - Determine direction

        ostringstream stream;
        stream << from << "," << to << "," << fixed << setprecision(2) << loadingPercent << "," << direction;
        loadingResults.push_back(stream.str());
    }

    return loadingResults;
}

void load_flow::outputLineData()
{
    vector<string> lineLoadings = this->calculateLineLoading(); // JK - Get line loadings
    fstream fout;
    fout.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\Output_lines.csv", ios::out);//"C:\\Users\\joran\\OneDrive\\Bureaublad\\Godot_v4.2.2-stable_mono_win64\\gdextension_cpp_example\\src\\Output_lines.csv", ios::out); // JK - Hardcoded file again
    if (!fout.is_open())
    {
        cerr << "Error: Output lines data file does not exist or cannot be opened in load_flow.hpp.\n";
        exit(1);
    }
    else
    {
        for (const auto& line : lineLoadings) {
            fout << line << "\n";
        }
        fout.close();
        return;
    }
}

void load_flow::output()
{
    fstream fout;
    fout.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\Output.csv", ios::out);//"C:\\Users\\joran\\OneDrive\\Bureaublad\\Godot_v4.2.2-stable_mono_win64\\gdextension_cpp_example\\src\\Output.csv", ios::out); // JK - Hardcoded file again
    if (!fout.is_open())
    {
        cerr << "Error: Output data file does not exist or cannot be opened in load_flow.hpp.\n";
        exit(1);
    }
    else
    {
        fout << this->header << "\n";
        for (int i = 0; i < this->N; i++)
        {
            for (int j = 0; j < this->busData[i].size(); j++)
                fout << this->busData[i][j] << ",";
            fout << "\n";
        }
        return;
    }
}

/* solves the load flow problem using gauss-seidel iterative method.
    updates the bus data.
    final result is stored in output.csv
 */
void load_flow::solveLoadFlow() // JK - This does as it suggests, it solves the load flow
{
    bool type_change;
    double Q, Qg;
    // logic
    for (int i = 1; i <= this->iterations; i++)
    {
        //cout << "Iteration " << i << " Start" << endl;
        this->V[1].push_back(complex<double>(1, 0));
        for (int j = 2; j <= this->N; j++)
        {
            //cout << "Updating Bus " << j << endl;
            switch ((int)this->busData[j - 1][0])
            {
            case 2: // pv // Load
                type_change = true;
                Q = this->reactive_power(i - 1, j);
                Qg = Q + this->busData[j - 1][6]; // Qg = Q + Ql

                if (Qg > this->busData[j - 1][7]) // Qg > Qgmax
                    Q = this->busData[j - 1][7];
                else if (Qg < this->busData[j - 1][8]) // Qg < Qgmin
                    Q = this->busData[j - 1][8];
                else
                    type_change = false;
                this->S[j].imag(Q);
                this->V[j].push_back(this->guass_seidel(i, j, 0));
                if (!type_change) // Qg does not exceed its limits
                {
                    this->V[j][i] = polar(abs(this->V[j][0]), arg(this->V[j][i]));
                    this->V[j][i] = this->guass_seidel(i, j, 1);
                    this->V[j][i] = polar(abs(this->V[j][0]), arg(this->V[j][i]));
                }
                else // Qg exceeded its limits
                    this->V[j][i] = this->guass_seidel(i, j, 1);
                break;

            case 3: // pq // Gen
                this->V[j].push_back(this->guass_seidel(i, j, 0));
                this->V[j][i] = this->guass_seidel(i, j, 1);
                break;
            default:
                break;
            }
        }
        // cout << "Iteration " << i << " End" << endl;
    }

    // P and Q for slack bus
    this->S[1].real(this->real_power(this->iterations, 1));
    this->S[1].imag(this->reactive_power(this->iterations, 1));
    //JK - Assign the slack load to buses that can handle it
    this->assignSlackLoad();

    this->update(); // updating busData
/*     this->printBusData(); */
    this->output();
    this->outputLineData(); // Calculate and print line loading after each iteration
/*     for (int k = 1; k <= N; k++){
        cout << "Power of node " << k << ": " << S[k] << " Voltage of node " << k << ": " << V[k].back() << endl;
    } */
    return;
}

void load_flow::assignSlackLoad() //JK - Function to assign slack load based on weights
{
    double totalSlackWeight = 0.0;
    for (int i = 0; i < this->N; i++)
    {
        if (this->canBeSlack[i + 1])
        {
            totalSlackWeight += this->slackWeights[i + 1]; //JK - total slack weight
        }
    }

    complex<double> totalSlack = this->S[1]; //JK - Total slack power

    for (int i = 0; i < this->N; i++)
    {
        double slackPercentage = this->slackWeights[i] / totalSlackWeight;
        if (this->canBeSlack[i + 1] && i > 0)
        {
            this->S[i + 1] += totalSlack * slackPercentage; //JK - Add slack load to existing value
        }
        else{
            this->S[i + 1] = totalSlack * slackPercentage;
        }
    }
    return;
}
#endif
