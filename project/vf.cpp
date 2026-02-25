#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace cv;


struct EuroRef {
    double value;
    double diameter;
    string colorGroup; 
};

vector<EuroRef> euroRefs = {
    {0.01, 16.25, "Cuivre"}, {0.02, 18.75, "Cuivre"}, {0.05, 21.25, "Cuivre"},
    {0.10, 19.75, "Or"},     {0.20, 22.25, "Or"},     {0.50, 24.25, "Or"},
    {2.00, 25.75, "Or"},     {1.00, 23.25, "Argent"} 
};


double getGrayStdDev(const Mat& gray) {
    Scalar mean, stddev;
    meanStdDev(gray, mean, stddev);
    return stddev[0];
}

vector<Vec3f> filterOverlappingCircles(const vector<Vec3f>& circles) {
    vector<Vec3f> filtered;
    for (const auto& current : circles) {
        bool isOverlapTooBig = false;
        for (const auto& kept : filtered) {
            double dist = norm(Point2f(current[0], current[1]) - Point2f(kept[0], kept[1]));
            if (dist < (current[2] + kept[2]) * 0.70) {
                isOverlapTooBig = true;
                break;
            }
        }
        if (!isOverlapTooBig) filtered.push_back(current);
    }
    return filtered;
}


vector<Vec3f> segment(Mat& src) {
    if (src.empty()) return {};

    Mat gray, blur;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    double stddev = getGrayStdDev(gray);
    int p1, p2, minDist;

    //filtrage depend de la texture du fond
    if (stddev > 44.0) { // Fond Marbré
        bilateralFilter(gray, blur, 11, 85, 85);
        p1 = 110; p2 = 70; minDist = 60;
    } 
    else if (stddev > 22.0) { // Fond granuleux
        Mat temp;
        medianBlur(gray, temp, 5); 
        GaussianBlur(temp, blur, Size(7, 7), 1.5);
        p1 = 95; p2 = 42; minDist = 45;
    }
    else { // Fond uni
        GaussianBlur(gray, blur, Size(11, 11), 2);
        p1 = 80; p2 = 35; minDist = 40;
    }

    vector<Vec3f> circles;
    HoughCircles(blur, circles, HOUGH_GRADIENT, 1.0, minDist, p1, p2, 20, 350);
    return filterOverlappingCircles(circles);
}


std::pair<cv::Mat, std::pair<int, double>> identifyAndDraw(Mat src) {
    if (src.empty()) return {src, {0, 0.0}};

    //Redimensionnement
    Mat img;
    float scale = 1024.0f / max(src.cols, src.rows);
    resize(src, img, Size(), scale, scale, INTER_AREA);

    //segmentation adaptative
    vector<Vec3f> circles = segment(img);

    int countFinal = 0;
    double totalMoney = 0.0;
    
    vector<double> detectedDiameters;
    vector<Point> detectedCenters;
    vector<string> detectedTypes;

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    for (const auto& c : circles) {
        Point center(cvRound(c[0]), cvRound(c[1]));
        double radius = c[2];

        if (center.x - radius < 0 || center.y - radius < 0 ||
            center.x + radius >= img.cols || center.y + radius >= img.rows)
            continue;

        // 65% du cercle pour tomber dans le centre
        Mat mask = Mat::zeros(img.size(), CV_8U);
        circle(mask, center, cvRound(radius * 0.65), Scalar(255), -1);
        Scalar avg = mean(hsv, mask);
        
        string type = "Or";
        if (avg[1] < 45) type = "Argent"; // Faible saturation
        else if (avg[0] < 20 || avg[0] > 160) type = "Cuivre"; // Teintes rouges

        detectedDiameters.push_back(radius * 2.0);
        detectedCenters.push_back(center);
        detectedTypes.push_back(type);
    }

    //Compare par rapport a la matrice des ratios et choisit le plus proche
    for (size_t i = 0; i < detectedDiameters.size(); i++) {
        double bestMatchValue = 0;
        double minTotalError = 1e9; 

        for (const auto& ref : euroRefs) {
            if (ref.colorGroup != detectedTypes[i]) continue;

            double currentError = 0;
            for (size_t j = 0; j < detectedDiameters.size(); j++) {
                if (i == j) continue;
                
                double measuredRatio = detectedDiameters[i] / detectedDiameters[j];
                double bestLocalError = 1e9;
                
                for (const auto& refJ : euroRefs) {
                    double theoreticalRatio = ref.diameter / refJ.diameter;
                    double error = abs(measuredRatio - theoreticalRatio);
                    if (error < bestLocalError) bestLocalError = error;
                }
                currentError += bestLocalError;
            }

            if (currentError < minTotalError) {
                minTotalError = currentError;
                bestMatchValue = ref.value;
            }
        }

        if (bestMatchValue > 0) {
            totalMoney += bestMatchValue;
            countFinal++;

            // Dessin
            Scalar drawCol = (detectedTypes[i] == "Cuivre") ? Scalar(0, 120, 255) : 
                             (detectedTypes[i] == "Argent") ? Scalar(200, 200, 200) : Scalar(0, 200, 255);
            
            circle(img, detectedCenters[i], cvRound(detectedDiameters[i]/2.0), drawCol, 3);
            string valStr = to_string(bestMatchValue).substr(0, 4) + "E";
            putText(img, valStr, detectedCenters[i] + Point(-20, 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        }
    }

    return {img, {countFinal, totalMoney}};
}

string cleanString(string s) {
    // Supprime les espaces au début et à la fin
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
    // Supprime les \r résiduels (cas classique des CSV Windows sur Linux)
    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
    return s;
}


int main() {
    ifstream file("data_annot.csv");

    if (!file.is_open()) {
        cout << "Impossible d'ouvrir le fichier CSV !" << endl;
        return -1;
    }

    string line;
    int total = 0;
    int correct = 0;
    int correctNb = 0;
    int correctVal = 0;

    // Structures pour les stats détaillées par groupe
    std::map<string, int> totalParGroupe;
    std::map<string, int> correctParGroupe;  
    std::map<string, int> correctNbParGroupe; 
    std::map<string, int> correctValParGroupe;

    while (getline(file, line)) {
        stringstream ss(line);
        string nom1, nb, val, grp;

        getline(ss, nom1, ',');
        getline(ss, nb, ',');
        getline(ss, val, ',');
        getline(ss, grp, ',');

        int gtNb = stoi(nb);
        double gtVal = stod(val);
        string nom = cleanString(nom1);
        string groupe = cleanString(grp);
        string path = "data/" + groupe + "/" + nom;
        cout<<groupe<<" - "<<nom<<endl;
        Mat img = imread(path);
        if (img.empty()) continue;

        auto output = identifyAndDraw(img);
        int detectedNb = output.second.first;
        double detectedVal = output.second.second;
        bool nbOK = (detectedNb == gtNb);
        bool valOK = (fabs(detectedVal - gtVal) < 0.01);

        total++;
        totalParGroupe[groupe]++; 

        if (nbOK) {
            correctNb++;
            correctNbParGroupe[groupe]++;
        }
        if (valOK) {
            correctVal++;
            correctValParGroupe[groupe]++;
        }
        if (nbOK && valOK) {
            correct++;
            correctParGroupe[groupe]++;
        }
        cout << "GT [ "<<gtNb<<" , "<<gtVal<<" ] - Pred [ "<<detectedNb<<" , "<<detectedVal<<" ]" << endl;

        cout << "--------------------------------------" << endl;

    } 
    file.close();

    
    cout << "\n========================================================" << endl;
    cout << "             RAPPORT DE PRECISION PAR GROUPE" << endl;
    cout << "========================================================" << endl;
    cout << left << setw(15) << "Groupe" 
        << "| Prec. NB " 
        << "| Prec. VAL" 
        << "| Prec. TOTALE (Nb+Val)" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (auto const& x : totalParGroupe) {
        string id = x.first;
        int totalG = x.second;
        
        double pNb  = (100.0 * correctNbParGroupe[id]) / totalG;
        double pVal = (100.0 * correctValParGroupe[id]) / totalG;
        double pTot = (pNb + pVal) / 2;

        cout << left << setw(15) << id 
            << "| " << setw(8) << pNb << "% "
            << "| " << setw(8) << pVal << "% "
            << "| " << pTot << "%" << endl;
    }

    cout << "========================================================" << endl;
    cout << "PRECISION GLOBALE DU SYSTEME : " << (total > 0 ? 100.0 * correct / total : 0) << " %" << endl;
    cout << "========================================================" << endl;

    cout << "\nENTREE pour quitter" << endl;
    cin.clear(); 
    cin.sync();
    cin.get();
    

    return 0;
}

