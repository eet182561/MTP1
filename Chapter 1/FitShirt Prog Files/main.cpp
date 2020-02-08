#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
using namespace std;
/*
 * The Shirt has been divided in to sectors. The torso, the arms and the shoulders. Code for each sector is written
 *  separately.
 */


/*
 * The function below generates the model of the shirt. The model contains three structures or the sectors
 */
struct torso{
    //It is a rectangle
    double l;
    double b;

};

struct shoulder{
    //It is rectangle;
    double l;
    double b;
};

struct arms{
    //It is trapezium
    double a,b,h;
};

struct patch{
    //Square patch
    double a;
};
struct model{
    //Each shirt has 1 torso, 2 shoulders and 2 arms
    torso t;
    shoulder s;
    arms a;
};

model generate(const string Size){

    //Following are the dimension of small shirt
    //Torso
    int l_t= 30;
    int b_t = 20;

    //Shoulder
    int l_s = 5;
    int b_s = 5;
    //Arms
    int a_a = 2;
    int b_a = 10;
    int h_a = 34;

    //For different sizes simple scaling is done.
    double scale_x,scale_y;

    if(Size=="S"){
        scale_x = 1; scale_y=1;
    }else if(Size=="M"){
        scale_x = 1.01;
        scale_y = 1.01;
    }else if(Size=="L"){
        scale_x = 1.02;
        scale_y = 1.02;
    }else{
        scale_x = 1.03;
        scale_y = 1.03;
        //Size == XL
    }
    return model{
            {l_t*scale_y,b_t*scale_y},
            {l_s*scale_y,b_s*scale_x},
            {a_a*scale_y,b_a*scale_y,h_a*scale_x}
    };
}

struct fit_rval{
    double pf;
    double area;
    double total_area;
};

fit_rval fit_torso(torso t,patch p){
    //Function to fit torso
    double len_covered = floor(t.l/p.a); //Length Covered
    double bre_covered = floor(t.b/p.a); //Breadth Covered
    return {double(len_covered*bre_covered*p.a*p.a)/double(t.l*t.b),len_covered*bre_covered*p.a*p.a,t.l*t.b};
}

fit_rval fit_shoulder(shoulder s, patch p){
    double len_covered = floor(s.l/p.a); //Length Covered
    double bre_covered = floor(s.b/p.a); //Breadth Covered
    return {float(len_covered*bre_covered)/float(s.l*s.b),len_covered*bre_covered*p.a*p.a,s.l*s.b};
}
struct fit_trep_rval{
    arms trep;
    double area;
};
double fit_trep(patch p,arms a,double &area){
    if(p.a>a.h) return area;
    else{
        int t_1 = a.a/p.a;
        double x = a.h*a.a/(a.b-a.a);
        if(t_1==0){
            double d = x/a.a*(p.a-1);
            double d_prime = ceil(d/p.a)*p.a;
            //cout<<"t_1: "<<t_1<<"d_prime: "<<d_prime<<endl;
            area+= t_1*p.a*d_prime;
            double new_a = (d_prime+x)/x*a.a;
            return fit_trep(p,{new_a,a.b,a.h-d_prime},area);
        }

        double d = x/a.a*((t_1+2)*p.a-1);
        double d_prime = ceil(d/p.a)*p.a;
        //cout<<"t_1: "<<t_1<<" d: "<<d<<" d_prime: "<<d_prime<<endl;
        area+= t_1*p.a*(d_prime>a.h?floor(a.h/p.a)*p.a:d_prime);
        double new_a = (d_prime+x)/x*a.a;
        return fit_trep(p,{new_a,a.b,a.h-d_prime},area);
    }
}
fit_rval fit_arms(arms ar,patch p){
    double area = 0;
    area = fit_trep(p,ar,area);
    double trep_area = 0.5*(ar.a+ar.b)*ar.h;
    return {area/trep_area,area,trep_area};
}

//Debugging funcitons for help
void print_model(model a){
    cout<<"The dimension of the sirt are:"<<endl;
    cout<<"Torso:\t"<<a.t.l<<" x "<<a.t.b<<endl;
    cout<<"Arms :\t"<<a.a.a<<" x "<<a.a.b<<" x "<<a.a.h<<endl;
    cout<<"Shoul:\t"<<a.s.l<<" x "<<a.s.b<<endl;
    cout<<endl;
}

void print_fit_rval(fit_rval fr){
    cout<<"PF  = "<<fr.pf<<endl;
    cout<<"AOP = "<<fr.area<<endl;
    cout<<"ToA = "<<fr.total_area<<endl;
    cout<<endl;
}

fit_rval fit(model shirt,patch p){
    fit_rval tor_fit = fit_torso(shirt.t,p);
    fit_rval sho_fit = fit_shoulder(shirt.s,p);
    fit_rval arm_fit = fit_arms(shirt.a,p);
    double tot_area = (tor_fit.area + sho_fit.area + arm_fit.area)*2;
    double area_shirt = (tor_fit.total_area + sho_fit.total_area + arm_fit.total_area)*2;
    return {tot_area/area_shirt,tot_area,area_shirt};
}

void display_results(vector<vector<double>> pfs,vector<string> sizes){
    for(int i=0;i<4;i++){
        cout<<"****** "<<sizes[i]<<" **********"<<endl;
        cout<<"patch_size\tpf"<<endl;
        for(int k=0;k<20;k++){
            cout<<(k+1)/2.0<<"\t\t\t"<<pfs[i][k]<<endl;
        }
        cout<<endl;
    }
}
void print_result(vector<vector<double>> pfs,vector<string> sizes){
    ofstream fout;

    for(int i=0;i<4;i++){
        fout.open("Output_"+sizes[i]+".dat");
        model a = generate(sizes[i]);
        fout<<"#The dimension of the shirt "<<sizes[i]<<" are:"<<endl;
        fout<<"#Torso:\t"<<a.t.l<<" x "<<a.t.b<<endl;
        fout<<"#Arms :\t"<<a.a.a<<" x "<<a.a.b<<" x "<<a.a.h<<endl;
        fout<<"#Shoul:\t"<<a.s.l<<" x "<<a.s.b<<endl;
        fout<<endl;
        fout<<"#****** "<<sizes[i]<<" **********"<<endl;
        fout<<"#patch_size\tpf"<<endl;
        for(int k=0;k<20;k++){
            fout<<(k+1)/2.0<<"\t\t\t"<<pfs[i][k]<<endl;
        }
        fout<<endl;
        fout.close();
    }

}
int main() {
    std::cout << "Hello, World!" << std::endl;
    vector<string> sizes = {"S","M","L","XL"};
    vector<vector<double>> pfs;
    for (const auto s:sizes){
        vector<double> temp;
        model ss = generate(s);
        for(double i=0.5;i<=10; i+=0.5){
            patch p{i};
            temp.push_back(fit(ss,p).pf);
        }
        pfs.push_back(temp);
    }
    display_results(pfs,sizes);
    print_result(pfs,sizes);
    return 0;
}

