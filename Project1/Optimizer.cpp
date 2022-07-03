// Optimizer.cpp: implementation of the COptimizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Optimizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptimizer::COptimizer(CLdjTextFile *Fichier, CHPPASCALCompilerCtrl *dialogue)
{
	dlg = dialogue;

	// Chargement du fichier source
	Chargement (Fichier);

	// Initialisation
	FonctionsUtilisees.RemoveAll();

	// Effectue la traduction
	Traduction ();

	// Sauvegarde les modifications
	Sauvegarde();
}

COptimizer::~COptimizer()
{

}

void COptimizer::Chargement(CLdjTextFile *Fichier)
{
	Fichier->StartReading();

	CList <TInstruction, TInstruction&> temp;
	temp.RemoveAll();

	// Initialisation de la liste d'instructions
	ListeInstructions.RemoveAll();

	int size = Fichier->GetLength();
	int ofs = 0;
	
	// Lecture du fichier
	while (!Fichier->eof())
	{
		// Lit une chaine
		CString chaine = "";
		char octet = 0;
		while (octet!='\r')
		{
			octet = Fichier->Read();
			ofs++;
			if (ofs%50==0) 
			{
				dlg->EtapeJauge(ofs*50/size);
				dlg->Update();
			}
			if (octet=='\r') break;
			chaine+=octet;
		}

		//	Rempli les champs de TInstruction
		TInstruction current;
		current.op=chaine;
		current.arg1="";
		current.arg2="";
		current.arg3="";
		current.arg4="";

		chaine += ":";
		if (chaine.Find((CString)":")!=-1) { current.op=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (current.op=="DATA") { current.arg1=chaine.Left(chaine.GetLength()-1); chaine = "";}
		
		if (chaine.Find((CString)":")!=-1) { current.arg1=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg2=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg3=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg4=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }

		if ((current.op=="CONST") || (current.op=="ENDCONST") || (current.op=="DATA"))
			temp.AddTail (current);
		else
			ListeInstructions.AddTail (current);

		// On passe à la ligne suivante
		octet=Fichier->Read();
		ofs++;
		if (ofs%50==0) 
		{
			dlg->EtapeJauge(ofs*50/size);
			dlg->Update();
		}
	}

	POSITION position = temp.GetHeadPosition();
	if (position!=NULL) while (true)
	{
		TInstruction current= temp.GetAt(position);
		ListeInstructions.AddTail (current);
		if (position==temp.GetTailPosition()) break;
		temp.GetNext(position);	
	}
}

void COptimizer::Sauvegarde()
{
	generatedFile = new CLdjTextFile (dlg->directoryDestination(), "Resultat3.txt");

	POSITION position = ListeInstructions.GetHeadPosition();

	TInstruction Ins1, Ins2, Ins3;
	Ins1.op=""; Ins2.op=""; Ins3.op="";

	bool fini = false;
	bool flag = true;

	int size = ListeInstructions.GetCount();
	int count = 0;

	while (!fini)
	{
		if (position==ListeInstructions.GetTailPosition()) fini=true;
		
		Ins3 = ListeInstructions.GetNext(position);

		count++;
		if (count%50==0) 
		{
			dlg->EtapeJauge(50+count*50/size);
			dlg->Update ();
		}

		// Optimisations
		// -------------

		// Multiplication et division d'entiers par une cste
		if ((Ins1.op=="CST") && ((Ins1.arg1=="BYTE") || (Ins1.arg1=="INTEGER")) && (Ins2.op=="OP") && (Ins2.arg2=="*"))
		{
			Ins2.op="HARDMULT";
			Ins2.arg1=Ins1.arg1;
			Ins2.arg2=Ins1.arg2;
			Ins1.op="";
		}

		// Elimination pure et simple de code (recherche d'elts neutres)
		if ((Ins1.op=="CST") && (Ins1.arg2=="0") && (Ins2.op=="OP") && ((Ins2.arg2=="+")||(Ins2.arg2=="-"))) { Ins1.op=""; Ins2.op=""; }
		if ((Ins1.op=="CST") && (Ins1.arg2=="1") && (Ins2.op=="OP") && ((Ins2.arg2=="*")||(Ins2.arg2=="/")||(Ins2.arg2=="DIV"))) { Ins1.op=""; Ins2.op=""; }
		if ((Ins1.op=="CST") && (Ins1.arg2=="TRUE") && (Ins2.op=="OP") && (Ins2.arg2=="AND")) { Ins1.op=""; Ins2.op=""; }
		if ((Ins1.op=="CST") && (Ins1.arg2=="FALSE") && (Ins2.op=="OP") && (Ins2.arg2=="OR")) { Ins1.op=""; Ins2.op=""; }

		// Optimisation des DUP avec les cste
		if ((Ins1.op=="CST") && (Ins2.op=="DUP")) Ins2=Ins1;
		
		// Optimisation des NEG+GOYES/GONO
		if ((Ins1.op=="OP") && (Ins1.arg2=="NEG") && (Ins2.op=="GOYES")) { Ins1.op=""; Ins2.op="GONO"; }
		if ((Ins1.op=="OP") && (Ins1.arg2=="NEG") && (Ins2.op=="GONO")) { Ins1.op=""; Ins2.op="GOYES"; }

		// Optimisation des CSTES+CONVERSION
		if ((Ins1.op=="CST") && ((Ins1.arg1=="BYTE") || (Ins1.arg1=="INTEGER")) && (Ins2.op=="CV") && ((Ins2.arg1=="REAL") || (Ins2.arg1=="INTEGER"))) { Ins1.arg1=Ins2.arg2; Ins2=Ins1; Ins1.op=""; }

		// Effectue les opérations entre constantes
		if ((Ins1.op=="CST") && (Ins2.op=="CST") && (Ins3.op=="OP"))
		{
			// opération UNaire
			if (Ins3.arg2=="NEG")
			{
				if ((Ins1.arg1=="BYTE") || (Ins1.arg1=="INTEGER"))
				{					
					int n=StrToInt(Ins2.arg2.GetString());
					Ins2.arg2=Ins1.arg2; 
					Ins1.op=""; 
					Ins3.op="CST"; Ins3.arg2=IntToStr(-n);					
				}
				if ((Ins1.arg1=="FLOAT"))
				{					
					double n=StrToFloat(Ins2.arg2.GetString());
					Ins2.arg2=Ins1.arg2;
					Ins1.op=""; 
					Ins3.op="CST"; Ins3.arg2=FloatToStr(-n);
				}
			}
			else
			{
				// opération BINaire
				if ((Ins1.arg1=="BYTE") || (Ins1.arg1=="INTEGER"))
				{
					int n1=StrToInt(Ins1.arg2.GetString());
					int n2=StrToInt(Ins2.arg2.GetString());
					if (Ins3.arg2=="+") n1+=n2;
					if (Ins3.arg2=="-") n1-=n2;
					if (Ins3.arg2=="*") n1*=n2;
					if (Ins3.arg2=="DIV") n1/=n2;
					if (Ins3.arg2=="MOD") n1%=n2;
					Ins1.op=""; Ins2.op=""; 
					Ins3.op="CST"; Ins3.arg2=IntToStr(n1);
				}
				if ((Ins1.arg1=="FLOAT"))
				{
					double n1=StrToFloat(Ins1.arg2.GetString());
					double n2=StrToFloat(Ins2.arg2.GetString());
					if (Ins3.arg2=="+") n1+=n2;
					if (Ins3.arg2=="-") n1-=n2;
					if (Ins3.arg2=="*") n1*=n2;
					if (Ins3.arg2=="/") n1/=n2;				
					Ins1.op=""; Ins2.op=""; 
					Ins3.op="CST"; Ins3.arg2=FloatToStr(n1);
				}
			}
		}

		// Ecrit la ligne la plus ancienne
		// -------------------------------

		if (Ins1.op!="")
		{
			CString chaine = Ins1.op;			
			if (Ins1.arg1!="") chaine +=":"+Ins1.arg1;
			if (Ins1.arg2!="") chaine +=":"+Ins1.arg2;
			if (Ins1.arg3!="") chaine +=":"+Ins1.arg3;
			if (Ins1.arg4!="") chaine +=":"+Ins1.arg4;
			chaine += "\r\n";
			
			if ((Ins1.op=="PROC") && (FonctionsUtilisees.Find(Ins1.arg1)==NULL)) flag=false;
			
			if (flag) generatedFile->Write(chaine);
			
			if (Ins1.op=="ENDPROC") flag=true;
		}
		
		// Rotation des lignes du buffer

		Ins1=Ins2;
		Ins2=Ins3;
	}

	// Vide le buffer temporaire

	dlg->EtapeJauge (100);
	dlg->Update ();

	CString chaine = Ins1.op;
	if (Ins1.arg1!="") chaine +=":"+Ins1.arg1;
	if (Ins1.arg2!="") chaine +=":"+Ins1.arg2;
	if (Ins1.arg3!="") chaine +=":"+Ins1.arg3;
	if (Ins1.arg4!="") chaine +=":"+Ins1.arg4;
	chaine += "\r\n";
	if (flag) generatedFile->Write(chaine);
	
	Ins1=Ins2;

	chaine = Ins1.op;
	if (Ins1.arg1!="") chaine +=":"+Ins1.arg1;
	if (Ins1.arg2!="") chaine +=":"+Ins1.arg2;
	if (Ins1.arg3!="") chaine +=":"+Ins1.arg3;
	if (Ins1.arg4!="") chaine +=":"+Ins1.arg4;
	chaine += "\r\n";
	if (flag) generatedFile->Write(chaine);
}


//////////////////////////////////////////////////////////////////////
// Optimisation proprement dite
//////////////////////////////////////////////////////////////////////

void COptimizer::Traduction ()
{
/*
	while (true)
	{
		POSITION positionSwap = Recherche ("SWAP");
		if (positionSwap == NULL) break;

		// Gestion des "SWAP"
		
		POSITION positionBloc1, positionBloc2;

		POSITION position = positionSwap;
		ListeInstructions.GetPrev (position);

		int PileLevel = 0;

		while (PileLevel!=1)
		{
			positionBloc2=position;
			TInstruction current = ListeInstructions.GetPrev(position);
			PileLevel+=DecalagePile(current);
		}

		ListeInstructions.GetPrev (positionBloc2);

		while (PileLevel!=2)
		{
			positionBloc1=position;
			TInstruction current = ListeInstructions.GetPrev(position);
			PileLevel+=DecalagePile(current);
		}
		
		Deplace (positionBloc1, positionBloc2, positionSwap);
		ListeInstructions.RemoveAt(positionSwap);		
	}
*/

	// Mémorisation des procédures et fonctions utilisées

	{
		POSITION position = Recherche ("MAIN");
		while (true)
		{
			TInstruction current = ListeInstructions.GetAt(position);
			if (current.op=="END") break;
			if (current.op=="CALL") if (FonctionsUtilisees.Find(current.arg1)==NULL) FonctionsUtilisees.AddTail(current.arg1);
			ListeInstructions.GetNext(position);
		}
	}

	POSITION positionStr = FonctionsUtilisees.GetHeadPosition();
	while (true)
	{
		if (positionStr==NULL) break;

		POSITION position = Recherche ("PROC", FonctionsUtilisees.GetAt(positionStr));
		while (true)
		{
			TInstruction current = ListeInstructions.GetAt(position);
			if (current.op=="ENDPROC") break;
			if (current.op=="CALL") if (FonctionsUtilisees.Find(current.arg1)==NULL) FonctionsUtilisees.AddTail(current.arg1);
			ListeInstructions.GetNext(position);
		}
		FonctionsUtilisees.GetNext(positionStr);
	}
}

// ---------------------
// Fonctions auxiliaires
// ---------------------

POSITION COptimizer::Recherche (CString Instruction)
{
	POSITION position = ListeInstructions.GetHeadPosition();
	
	while (true)
	{
		TInstruction current = ListeInstructions.GetAt(position);
		if (current.op==Instruction) return (position);
		if (position==ListeInstructions.GetTailPosition()) break;
		ListeInstructions.GetNext(position);
	}
	return NULL;
}

POSITION COptimizer::Recherche (CString Instruction, CString arg1)
{
	POSITION position = ListeInstructions.GetHeadPosition();
	
	while (true)
	{
		TInstruction current = ListeInstructions.GetAt(position);
		if ((current.op==Instruction) && (current.arg1==arg1)) return (position);
		if (position==ListeInstructions.GetTailPosition()) break;
		ListeInstructions.GetNext(position);
	}
	return NULL;
}

void COptimizer::Deplace (POSITION SourceStart, POSITION SourceEnd, POSITION Destination)
{
	CList <TInstruction, TInstruction&> Sauvegarde;

	POSITION position = Destination;
	ListeInstructions.GetPrev(position);
	while (true)
	{
		TInstruction current = ListeInstructions.GetAt(position);
		Sauvegarde.AddHead(current);
		POSITION old=position;
		ListeInstructions.GetPrev(position);
		ListeInstructions.RemoveAt(old);
		if (position==SourceEnd) break;
	}

	while (true)
	{
		TInstruction current = Sauvegarde.GetHead();
		ListeInstructions.InsertBefore(SourceStart, current);
		Sauvegarde.RemoveHead();
		if (Sauvegarde.IsEmpty()) break;
	}
}

int COptimizer::DecalagePile (COptimizer::TInstruction current)
{
	if (current.op=="SAVE") return -2;
	if ((current.op=="OP") && (current.arg1=="COPY")) return -2;

	if ((current.op=="OP") && (current.arg1!="NEG") && (current.arg1!="LENGTH")) return -1;;
	if (current.op=="DROP") return -1;

	if (current.op=="CST") return 1;
	if (current.op=="DUP") return 1;	
	
	if (current.op=="CALL") 
		return -(StrToInt(current.arg2));

	return 0;
}