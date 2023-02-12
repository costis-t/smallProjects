import argparse
import glob
import openpyxl


# Iterator and Factory
class Spreadsheet:
    def __init__(self, filename: str, start_cell: int, end_cell: int):
        self.wb = openpyxl.load_workbook(filename, data_only=True)
        self.project_summary_sheet = self.wb.sheetnames[0]
        self.start_cell = start_cell
        self.end_cell = end_cell

    def __iter__(self):
        for cell in range(self.start_cell, self.end_cell + 1):
            title = self.wb[self.project_summary_sheet]['F' + str(cell)].value
            value = self.wb[self.project_summary_sheet]['G' + str(cell)].value
            yield (title, value)

def SpreadsheetFactory(filename: str, start_cell: int, end_cell: int) -> Spreadsheet:
    return Spreadsheet(filename, start_cell, end_cell)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Excel cell helper')
    parser.add_argument('pattern', type=str, help='Filename pattern')
    parser.add_argument('start', type=int, help='Range: start cell')
    parser.add_argument('end', type=int, help='Range: end cell')
    args = parser.parse_args()

    excel_files = glob.glob(args.pattern)
    for file in excel_files:
        print('--------------------')
        print(file)
        print('--------------------')
        excel_cells = SpreadsheetFactory(file, args.start, args.end)
        for title, value in excel_cells:
            print(f"{title}: {value}")
        print('\n')
